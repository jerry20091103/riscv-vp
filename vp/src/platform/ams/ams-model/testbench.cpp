#include <systemc-ams.h>
#include "heat_source.h"
#include "heat_system.h"
#include "adc_mapper.h"
#include "sink.h"
int sc_main(int argc, char *argv[]) {

// signals
sca_tdf::sca_signal<double> P_1("P_1");
sca_tdf::sca_signal<double> T_1("T_1");

// heat source
heat_source src_1("src_1");
src_1.heaterVal = 10.0;
src_1.P_out(P_1);

// temperature heating system
heat_system sys_1("sys_1");
sys_1.P_in(P_1);
sys_1.T_out(T_1);

// sinks (for console logging)
sink sink_1("sink_1");
sink_1.in(T_1); // Instantiate sink

// .dat and .vcd traces
sca_trace_file* tfp = sca_create_tabular_trace_file("testbench");
sca_trace(tfp, T_1, "T_1");
sca_trace(tfp, P_1, "P_1");

// systemc simulation
sc_start(20.0, SC_SEC);

// 50% duty cycle heating rate -- ~7.5°C
for(int i = 0; i<50; i++){
	src_1.heaterVal = 0.0;
	sc_start(0.3, SC_SEC);
	src_1.heaterVal = 10.0;
	sc_start(0.1, SC_SEC);
}
// 87.5% duty cycle heating rate -- ~22.5°C
for(int i = 0; i<50; i++){
	src_1.heaterVal = 0.0;
	sc_start(0.05, SC_SEC);
	src_1.heaterVal = 10.0;
	sc_start(0.35, SC_SEC);
}

// turn off heater
src_1.heaterVal = 0.0;	
sc_start(10.0, SC_SEC);

sca_close_tabular_trace_file(tfp); // Close trace file
return 0;

// Exit with return code 0
}
