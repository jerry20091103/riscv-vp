#ifndef ADC_MAPPER_H
#define ADC_MAPPER_H
#include <systemc-ams.h>
#include <cmath>

SCA_TDF_MODULE(adc_mapper)
{
	// signals
	sca_tdf::sca_in<double> T_analog_in;
	sca_tdf::sca_out<int> T_digital_out;
	sca_tdf::sca_out<int> T_dig_wp_out;
	sca_tdf::sca_out<int> T_dig_fp_out;
	
	sca_tdf::sca_de::sca_out<uint32_t> tOut;

	double mappedTemp;

	uint32_t sampleAndMap(double measuredTemp);	
	void initialize();
	void set_attributes(); // Set TDF attributes
	void processing(); // Describe time-domain behavior
	SCA_CTOR(adc_mapper) : T_analog_in("T_analog_in"), T_digital_out("T_digital_out")
	{

	}

};

#endif // ADC_MAPPER_H
