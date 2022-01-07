#include "heat_system.h"
#include <cmath>
#include "../timescales.h"
#include "../model_debug.h"
#include "../ams_params.h"

void heat_system::initialize()
{	
	deltaT = get_timestep().to_seconds();
	T_imo = T_0;
	T_i = T_0;
}
void heat_system::set_attributes() // Set TDF attributes
{
	// this could be made dependent on the 
	// properties of the simulated model
	// high inertia in heat (small P, high capacity, etc.)
	// could reduce the necessary sampling timestep a lot
	T_out.set_timestep(AMS_MODEL_TIMESTEP, SC_US);
}
void heat_system::processing()
{
	// double t = T_out.get_time().to_seconds(); // Get current time of the sample
	/*
	 * As commonly denoted with heating processes, T(t) assumes the 
	 * temperature difference T_ambient - T_airmass(t)
	 * 
	 * We begin with the heat-balance equation:
	 * 
	 * 		heat_in = heat_out + heat_stored
	 * 
	 * We replace the heat terms with the respective equations:
	 * 
	 * 		P = alpha*area*T(t) + m*c_p*d(T(t))/dt
	 * 
	 * This approximates a system of a heated mass m, with 
	 * area (and volume) specific heat capacity c_p, and its temperature 
	 * T(t) in dependence with the added heat P (i.e. from a heater)
	 * For a more realistic and dynamic system model, we can add time
	 * dependencies for the heater input P, the ambient temperature 
	 * T_ambient (in T(t)), and the airmass m inside the closed volume
	 * (as the mass m is dependent on the density of the air, which 
	 * itself is dependent on the temperature of the air).
	 * Further versions of this system model will approach modelling 
	 * these extensions.
	 * We further introduce the difference-equation transformation:
	 * 		
	 * 		T(t) = T_i
	 * 		d(T(t))/dt = (T_i+1 - T_i-1)/(2*deltaT)
	 * 
	 * And after replacing the terms in out differential equation after
	 * rearranging for T_i+1 we get:
	 * 
	 * 		T_i+1 = 2*deltaT/(m*c_p) * (P - alpha*area*T_i) + T_i-1
	 * 		
	 */
	T_ipo = ((2*deltaT)/(m*c_p))*(P_in.read() - alpha*area*T_i) + T_i;	
	
	#if AMS_DEBUG
	printf("[AMS] system @ %f = %f\n", this->get_time(), T_ipo);
	#endif
	
	T_out.write(T_ipo);
	// prepare next iteration step
	T_imo = T_i;
	T_i = T_ipo;
}
