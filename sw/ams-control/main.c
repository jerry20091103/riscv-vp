#include <stdio.h>
#include <stdbool.h>
#include "irq.h"
#define DEBUG 0
#define LOOPLIMIT 1000
uint32_t loopCnt = 0;

// memory mapped peripheral address
static volatile uint32_t * const SENSOR_VALUE_REG_ADDR = (uint32_t * const)0x50000000;
static volatile uint32_t * const HEATER_CTRL_REG_ADDR = (uint32_t * const)0x60000000;

// fsm states
typedef enum {
	COOLING = 0,
	HEATING = 1
} controllerState_t;

// state and i/o vars
controllerState_t currState = HEATING;
uint32_t T_raw = 0; 
int32_t T_wp = 0; // whole part
int32_t T_fp = 0; // ~~ unused for now ~~
bool b_heaterOn = false;

// sampling time = delay time between loop
int32_t const sampleN = 2000; // loop length in delay(n)

// control scenario vars
int32_t const temperatureThreshold = 1; // deg celsius
int32_t setpoint; // = 20; // deg celsius
//~ int32_t setpointB = 5; // deg celsius
uint32_t const holdTime = 10; // tics
//~ uint32_t holdTimeB = 10; // tics
uint32_t T_off; // = setpoint + temperatureThreshold;
uint32_t T_on; // = setpoint - temperatureThreshold;

_Bool has_sensor_data = 0;

void sensor_irq_handler() {
	has_sensor_data = 1;
}

uint32_t read_sensor() {
	return *SENSOR_VALUE_REG_ADDR;
}

void set_heater_off() {
	*HEATER_CTRL_REG_ADDR = 0;
}

void set_heater_on() {
	*HEATER_CTRL_REG_ADDR = 1;
}

void delay(uint32_t n) {
	for (int i=0; i<n; ++i) {
		asm("nop");
	}
}

void wait_for_sensor(uint32_t n) {
	while (n > 0) {
		--n;
		while (!has_sensor_data) {
			asm("wfi");
		}
		has_sensor_data = 0;
	}
}

void readInputVars() {
	T_raw = read_sensor(); // bits 31 to 1 = whole part, bit 0 = .5 degree increment
	T_wp = T_raw >> 1;  // whole part
	// ~~ unused for now ~~
	T_fp = T_raw & 0x1; // fraction part, 1 = .5 deg, 0 = .0 deg
	#if DEBUG
	printf("[SW] temp_value = %d %f\n", temp_wp, (temp_fp?0.5:0));
	#endif
	return;
}

void computeOutputVars() {	
	#if DEBUG
	printf("[SW] before: currState = %X \n", currState);
	printf("[SW] before: heaterOn = %X \n", b_heaterOn);
	#endif
	switch(currState) {
		case COOLING:
			// original guards
			//if(T_wp > T_on) {
			//if(T_wp <= T_on) {
			// injected fault: change (swap them actually) operator in both guards
			//if(T_wp <= T_on) {
			//if(T_wp > T_on) {
			if(T_wp > T_on) {
				currState = COOLING;
			}
			if(T_wp <= T_on) {
				currState = HEATING;
			}
		break;
		case HEATING:
			if(T_wp < T_off) {
				currState = HEATING;
			}
			if(T_wp >= T_off) {
				currState = COOLING;
			}
		break;
	}
	switch(currState) {
		case COOLING:
			b_heaterOn = false;
		break;
		case HEATING:
			b_heaterOn = true;
		break;
	}
	#if DEBUG
	printf("[SW] after: currState = %X \n", currState);
	printf("[SW] after: heaterOn = %X \n", b_heaterOn);
	#endif
	return;
}

void writeOutputVars(){
	if(b_heaterOn) {
		set_heater_on();
	} else {
		set_heater_off();
	}
	return;
}

void set_temperature_point(int32_t pt) {
	loopCnt=0;
	setpoint = pt; // deg celsius
	T_off = setpoint + temperatureThreshold;
    T_on = setpoint - temperatureThreshold;
	while(1){
		// fsm hystersis control loop
		readInputVars();
		computeOutputVars();
		writeOutputVars();
		//delay(sampleN);
		wait_for_sensor(1);
		// for simulation: calculate a fixed number of times
		loopCnt++;
		if(loopCnt >= LOOPLIMIT)
		{
			break;
		}
	}
	#if DEBUG
	printf("[SW] loopCnt = %u\n", loopCnt);
	#endif
}

int main() {
	register_interrupt_handler(2, sensor_irq_handler);

	// first setpoint at 20 degrees
	set_temperature_point(20);
	
	// second setpoint at 10 degrees
	set_temperature_point(10);
	
	// off + cooldown delay
	set_heater_off();
	//delay(300000);
	wait_for_sensor(5);
	return 0;
}
