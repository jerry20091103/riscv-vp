#pragma once

#include <systemc-ams.h>

struct Env {
	int sensor_value = 0;
	
	void set_heater_on() {
		// TODO: update module
		sensor_value = rand();
		//heatSignal = true;
		std::cout << "[VP] heater-on, sensor-value << " << sensor_value << std::endl;
	}
	
	void set_heater_off() {
		// TODO: update module
		sensor_value = 0;
		//~ heatSignal = true;
		std::cout << "[VP] heater-off, sensor-value << " << sensor_value << std::endl;
	}
	
	int get_current_sensor_value() {
		std::cout << "[VP] sensor-value << " << sensor_value << std::endl;
		return sensor_value;
	}
};
