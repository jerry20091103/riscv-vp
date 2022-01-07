#include "adc_mapper.h"
#include <cmath>
#include "../timescales.h"
#include "../model_debug.h"
void adc_mapper::initialize()
{
	
}
void adc_mapper::set_attributes() // Set TDF attributes
{
	// needs to fit with input timestep/sampling rate 
	T_digital_out.set_timestep(AMS_MODEL_TIMESTEP, SC_US);
	tOut.set_timestep(AMS_MODEL_TIMESTEP, SC_US);
}
void adc_mapper::processing()
{
	/*
	 *  turn analog temperature (from temperature source) into 
	 * register compatible 32-bit value (similar how a specific sensor
	 * returns the value)
	 * 
	 */
	uint32_t mappedTemp = sampleAndMap(T_analog_in);
	
	#if AMS_DEBUG
	printf("[AMS] sensor @ %f = %X\n", this->get_time(), mappedTemp);
	#endif
	
	// systemc ams implementation of a predefined function trace
	T_digital_out.write(mappedTemp);
	tOut.write(mappedTemp);
	// split fraction and whole part for vcd logging to additional outputs
	T_dig_fp_out.write((mappedTemp & 0x01) ? 5 : 0);
	T_dig_wp_out.write((mappedTemp >> 1));
}
uint32_t adc_mapper::sampleAndMap(double measuredTemp)
{
	// sampling characteristics similar to DS18S20 sensor
	double sensMinRange = -55.0; // °C
	double sensMaxRange = +85.0; // °C
	double cappedT = measuredTemp;
	double discreteT;
	uint16_t mappedT = 0;
	bool sign = std::signbit(measuredTemp);
	// cap temperature at ranges
	if(measuredTemp >= sensMaxRange) {
		cappedT = sensMaxRange;
	} else if(measuredTemp <= sensMinRange) {
		cappedT = sensMinRange;
	}
	// convert from measured value into sample in binary range
	discreteT = round(cappedT*2)/2; // round to nearest 0.5
	uint8_t intT = (uint32_t) discreteT; // get integer part
	bool fracT = (discreteT - intT) > 0; // get fractional part (0.5 or 0.0)
	mappedT = (intT << 1) | fracT; // build byte representation from ds18s20
	// signextend if necessary
	if(sign){
		mappedT = (0xff << 8) | mappedT;
	}
	return (uint32_t)mappedT;
}
