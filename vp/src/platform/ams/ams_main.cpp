#include <cstdlib>
#include <ctime>

#include "core/common/real_clint.h"
#include "elf_loader.h"
#include "debug_memory.h"
#include "iss.h"
#include "mem.h"
#include "memory.h"
#include "syscall.h"
#include "platform/common/options.h"
#include "fe310_plic.h"

#include <systemc-ams.h>
#include "sensor.h"
#include "heater.h"
#include "env.h"
#include "ams-model/heat_system.h"
#include "ams-model/adc_mapper.h"
#include "ams-model/heat_source.h"
#include "ams-model/sink.h"
#include "timescales.h"

#include "gdb-mc/gdb_server.h"
#include "gdb-mc/gdb_runner.h"

#include <boost/io/ios_state.hpp>
#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>

using namespace rv32;
namespace po = boost::program_options;

struct TinyOptions : public Options {
public:
	typedef unsigned int addr_t;

	addr_t mem_size = 1024 * 1024 * 32;  // 32 MB ram, to place it before the CLINT and run the base examples (assume
	                                     // memory start at zero) without modifications
	addr_t mem_start_addr = 0x00000000;
	addr_t mem_end_addr = mem_start_addr + mem_size - 1;
	addr_t clint_start_addr = 0x02000000;
	addr_t clint_end_addr = 0x0200ffff;
	addr_t sys_start_addr = 0x02010000;
	addr_t sys_end_addr = 0x020103ff;
	addr_t plic_start_addr = 0x40000000;
	addr_t plic_end_addr = 0x41000000;
	addr_t sensor_start_addr = 0x50000000;
	addr_t sensor_end_addr = 0x50001000;
	addr_t heater_start_addr = 0x60000000;
	addr_t heater_end_addr = 0x60001000;

	bool quiet = false;
	bool use_E_base_isa = false;

	TinyOptions(void) {
		// clang-format off
		add_options()
			("quiet", po::bool_switch(&quiet), "do not output register values on exit")
			("memory-start", po::value<unsigned int>(&mem_start_addr), "set memory start address")
			("memory-size", po::value<unsigned int>(&mem_size), "set memory size")
			("use-E-base-isa", po::bool_switch(&use_E_base_isa), "use the E instead of the I integer base ISA");
        	// clang-format on
        }

	void parse(int argc, char **argv) override {
		Options::parse(argc, argv);
		mem_end_addr = mem_start_addr + mem_size - 1;
	}
};

int sc_main(int argc, char **argv) {
	TinyOptions opt;
	opt.parse(argc, argv);

	std::srand(std::time(nullptr));  // use current time as seed for random generator

	//tlm::tlm_global_quantum::instance().set(sc_core::sc_time(opt.tlm_global_quantum, sc_core::SC_NS));
	tlm::tlm_global_quantum::instance().set(sc_core::sc_time(VP_TQ_TIME, sc_core::SC_NS));	
	
	// interconnect
	sca_tdf::sca_signal<double> P_1("P_1");
	sca_tdf::sca_signal<double> T_1("T_1");
	sca_tdf::sca_signal<int> T_dig_1("T_dig_1");
	sca_tdf::sca_signal<int> T_dig_wp_1("T_dig_wp_1");
	sca_tdf::sca_signal<int> T_dig_fp_1("T_dig_fp_1");
	sc_core::sc_signal<bool> heatSig("heatSig");
	sc_core::sc_signal<uint32_t> tempSig("tempSig");

	ISS core(0, opt.use_E_base_isa);
    MMU mmu(core);
	CombinedMemoryInterface core_mem_if("MemoryInterface0", core, &mmu);
	SimpleMemory mem("SimpleMemory", opt.mem_size);
	ELFLoader loader(opt.input_program.c_str());
	SimpleBus<2, 6> bus("SimpleBus");
	SyscallHandler sys("SyscallHandler");
	FE310_PLIC<1, 64, 96, 32> plic("PLIC");
	DebugMemoryInterface dbg_if("DebugMemoryInterface");
	Env env;
	TempSensor sensor("TempSensor", env, 2);
	sensor.tempIn(tempSig);
	Heater heater("Heater", env);
	heater.onSig(heatSig);
	
	// heat source
	heat_source src_1("src_1");
	src_1.heaterVal = 10.0;
	src_1.on(heatSig);
	src_1.P_out(P_1);
	// temperature heating system
	heat_system sys_1("sys_1");
	sys_1.P_in(P_1);
	sys_1.T_out(T_1);
	// sinks (for console logging)
	sink sink_1("sink_1");
	sink_1.in(T_1); // Instantiate sink
	// preliminary "adc sensor-layer"
	adc_mapper adc_1("adc_1");
	adc_1.T_analog_in(T_1);
	adc_1.tOut(tempSig);
	adc_1.T_digital_out(T_dig_1);
	adc_1.T_dig_wp_out(T_dig_wp_1);
	adc_1.T_dig_fp_out(T_dig_fp_1);
	// .dat and .vcd traces
	sca_trace_file* tfp = sca_create_tabular_trace_file("testbench");
	sca_trace(tfp, T_1, "T_1");
	sca_trace(tfp, P_1, "P_1");
	sca_trace(tfp, T_dig_wp_1, "T_dig_wp_1");
	sca_trace_file* atf = sca_create_vcd_trace_file("testbench_trace.vcd");
	sca_trace(atf, T_1, "T_1");
	sca_trace(atf, T_dig_1, "T_dig_1");
	sca_trace(atf, T_dig_wp_1, "T_dig_wp_1");
	sca_trace(atf, T_dig_fp_1, "T_dig_fp_1");
	sca_trace(atf, P_1, "P_1");
	sca_trace(atf, heatSig, "heatSig");
	
	std::vector<clint_interrupt_target*> clint_targets {&core};
	RealCLINT clint("CLINT", clint_targets);

	MemoryDMI dmi = MemoryDMI::create_start_size_mapping(mem.data, opt.mem_start_addr, mem.size);
	InstrMemoryProxy instr_mem(dmi, core);

	std::shared_ptr<BusLock> bus_lock = std::make_shared<BusLock>();
	core_mem_if.bus_lock = bus_lock;

	instr_memory_if *instr_mem_if = &core_mem_if;
	data_memory_if *data_mem_if = &core_mem_if;
	if (opt.use_instr_dmi)
		instr_mem_if = &instr_mem;
	if (opt.use_data_dmi) {
		core_mem_if.dmi_ranges.emplace_back(dmi);
	}

	loader.load_executable_image(mem, mem.size, opt.mem_start_addr);
	core.init(instr_mem_if, data_mem_if, &clint, loader.get_entrypoint(), rv32_align_address(opt.mem_end_addr));
	sys.init(mem.data, opt.mem_start_addr, loader.get_heap_addr());
	sys.register_core(&core);

	if (opt.intercept_syscalls)
		core.sys = &sys;

	// setup port mapping
	bus.ports[0] = new PortMapping(opt.mem_start_addr, opt.mem_end_addr);
	bus.ports[1] = new PortMapping(opt.clint_start_addr, opt.clint_end_addr);
	bus.ports[2] = new PortMapping(opt.sys_start_addr, opt.sys_end_addr);
	bus.ports[3] = new PortMapping(opt.sensor_start_addr, opt.sensor_end_addr);
	bus.ports[4] = new PortMapping(opt.heater_start_addr, opt.heater_end_addr);
	bus.ports[5] = new PortMapping(opt.plic_start_addr, opt.plic_end_addr);

	// connect TLM sockets
	core_mem_if.isock.bind(bus.tsocks[0]);
	dbg_if.isock.bind(bus.tsocks[1]);
	bus.isocks[0].bind(mem.tsock);
	bus.isocks[1].bind(clint.tsock);
	bus.isocks[2].bind(sys.tsock);
	bus.isocks[3].bind(sensor.tsock);
	bus.isocks[4].bind(heater.tsock);
	bus.isocks[5].bind(plic.tsock);
	
	// connect interrupt signals/communication
	plic.target_harts[0] = &core;
	sensor.plic = &plic;

	// switch for printing instructions
	core.trace = opt.trace_mode;

	std::vector<debug_target_if *> threads;
	threads.push_back(&core);

	if (opt.use_debug_runner) {
		auto server = new GDBServer("GDBServer", threads, &dbg_if, opt.debug_port);
		new GDBServerRunner("GDBRunner", server, &core);
	} else {
		new DirectCoreRunner(core);
	}

	if (opt.quiet)
		 sc_core::sc_report_handler::set_verbosity_level(sc_core::SC_NONE);

	printf("sim started \n");
	sc_core::sc_start();
	if (!opt.quiet) {
		core.show();
	}

	return 0;
}
