#ifndef HEAT_SYSTEM_H
#define HEAT_SYSTEM_H
#include <systemc-ams.h>
#include <cmath>
#include "../ams_params.h"

SCA_TDF_MODULE(heat_system)
{
	sca_tdf::sca_out<double> T_out;
	sca_tdf::sca_in<double> P_in;
	
	double T_imo, T_i, T_ipo; // [K] - temperatures
	//double heaterInput; // [W] - heat input (here, part of system)
	double m; // [kg] - m = heated air mass
	double c_p; // [J/(K*kg)] - c_p = specific heat capacity of air
	double alpha; // [W/(K*m^2)] heat transfer coefficient
	double area; // [m^2] - area of heated air volume-mass
	double deltaT; // [s] -- timestep dt for discretization
	double T_0; // [K] -- inital temperature, usually ambient
	
	void initialize();
	void set_attributes(); // Set TDF attributes
	void processing(); // Describe time-domain behavior
	
	/*
	 * Default values are for example scenario:
	 * 10cm cube (A=0.06m^2, V=0.001m^3), 0°C temperature difference to ambient, 
	 * 5 W heater, no dependencies of air with 
	 * density/temperature (therefore constant air mass inside cube)
	 * 
	 */ 
	SCA_CTOR(heat_system) : T_out("T_out"), T_0(init_T_0), m(airMass_m), 
	c_p(specificHeat_c_p), alpha(transferCoeff_alpha), area(airVol_area)	
	{

	}
};

#endif // HEAT_SYSTEM_H
