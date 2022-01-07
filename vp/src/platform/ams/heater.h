#pragma once

#include <cstdlib>
#include <cstring>

#include <systemc>

#include <tlm_utils/simple_target_socket.h>

#include "core/common/irq_if.h"

#include "env.h"

struct Heater : public sc_core::sc_module {
	tlm_utils::simple_target_socket<Heater> tsock;
	
	sc_core::sc_event run_event;
	
	sc_core::sc_out<bool> onSig;
	bool onState;
	
	Env &env;

	SC_HAS_PROCESS(Heater);
	
	Heater(sc_core::sc_module_name, Env &env) 
		: env(env) {
		tsock.register_b_transport(this, &Heater::transport);
		SC_THREAD(run);
	}

	void transport(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) {
		auto addr = trans.get_address();
		auto cmd = trans.get_command();
		auto len = trans.get_data_length();
		auto ptr = trans.get_data_ptr();
		
		assert (addr == 0);
		assert (len <= 4);
		assert (cmd == tlm::TLM_WRITE_COMMAND);

		auto ctrl = *ptr;
		
		#if TLM_DEBUG
		printf("[TLM] heater TLM write: %X\n", ctrl);
		#endif
		
		if (ctrl) {
			// HEATER is set on
			//~ env.set_heater_on();
			onState = true;
		} else {
			// HEATER is set off
			//~ env.set_heater_off();
			onState = false;
		}

		(void)delay;  // zero delay
	}
	
	void run(){
		while(true){
			run_event.notify(sc_core::sc_time(1, sc_core::SC_US));
			sc_core::wait(run_event);
			
			#if TLM_DEBUG
			printf("[TLM] heater thread write: %X\n", onState);
			#endif
			
			onSig.write(onState);
		}
	}
};
