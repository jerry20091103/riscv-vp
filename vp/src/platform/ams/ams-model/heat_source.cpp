#include "heat_source.h"
#include <cmath>
#include "../timescales.h"
#include "../model_debug.h"
#include "../ams_params.h"

void heat_source::initialize()
{
	
}
void heat_source::set_attributes() // Set TDF attributes
{
	// for constant values this is quite a bit funny
	P_out.set_timestep(AMS_MODEL_TIMESTEP, SC_US); 
	on.set_timestep(AMS_MODEL_TIMESTEP, SC_US);
}
void heat_source::processing()
{
	#if AMS_DEBUG
	printf("[AMS] heater @ %f = %X\n", this->get_time(), onState);
	#endif
	
	onState = on.read();
	if(onState) {
		P_out.write(heaterVal);
	} else {
		P_out.write(0.0);
	}
	
}
