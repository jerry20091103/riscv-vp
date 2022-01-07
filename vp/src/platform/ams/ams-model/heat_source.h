#ifndef HEAT_SOURCE_H
#define HEAT_SOURCE_H
#include <systemc-ams.h>
#include <cmath>
#include "../ams_params.h"

SCA_TDF_MODULE(heat_source)
{
	sca_tdf::sca_de::sca_in<bool> on;
	sca_tdf::sca_out<double> P_out;
	double heaterVal; // [W] -- heating power output
	bool onState;
	
	void initialize();
	void set_attributes(); // Set TDF attributes
	void processing(); // Describe time-domain behavior
	
	SCA_CTOR(heat_source) : P_out("P_out"), 
	heaterVal(source_heaterWatts), onState(false)
	{

	}
};

#endif // HEAT_SOURCE_H
