#pragma once

#include <cstdlib>
#include <cstring>

#include "core/common/irq_if.h"
#include "util/tlm_map.h"

#include "env.h"
#include "model_debug.h"
#include "timescales.h"

struct TempSensor : public sc_core::sc_module {
	tlm_utils::simple_target_socket<TempSensor> tsock;
	
	interrupt_gateway *plic = nullptr;
	uint32_t irq_number = 0;
	
	sc_core::sc_event run_event;
	
	sc_core::sc_in<uint32_t> tempIn;
	uint32_t tVal;
	
	SC_HAS_PROCESS(TempSensor);
	
	Env &env;

	TempSensor(sc_core::sc_module_name, Env &env, uint32_t irq_number) 
		: irq_number(irq_number), env(env) {
		tsock.register_b_transport(this, &TempSensor::transport);
		SC_THREAD(run);
	}

	void transport(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) {
		auto addr = trans.get_address();
		auto cmd = trans.get_command();
		auto len = trans.get_data_length();
		auto ptr = trans.get_data_ptr();
		
		assert (addr == 0);
		assert (len == 4);
		assert (cmd == tlm::TLM_READ_COMMAND);
		
		//~ *((uint32_t*)ptr) = env.get_current_sensor_value();
		// original behavior
		*((uint32_t*)ptr) = tVal;
		// introduce stuck at 1 bit at bit 1 (bit 0 is 0.5 degree increment)
		//~ *((uint32_t*)ptr) = tVal | 0x00000002;
		// introduce stuck at 1 bit at bit 2
		//*((uint32_t*)ptr) = tVal | 0x00000004;
		
		#if TLM_DEBUG
		printf("[TLM] sensor tlm read: %X\n", tVal);
		#endif
		
		(void)delay;  // zero delay
	}
	void run(){		
		while(true){
			run_event.notify(sc_core::sc_time(VP_SENSOR_SAMPLING_TIME, sc_core::SC_MS));
			sc_core::wait(run_event);
			tVal = tempIn.read();
			
			#if TLM_DEBUG
			printf("[TLM] sensor thread read: %X\n", tVal);
			#endif
			
			plic->gateway_trigger_interrupt(irq_number);
		}
	}
};
