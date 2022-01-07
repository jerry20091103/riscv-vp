#include "sink.h"
#include <iostream>
#include "../model_debug.h"
void sink::processing()
{
	#if AMS_DEBUG
	printf("[AMS] %s @ %d : %f\n", this->name(), this->get_time(), in.read());
	#endif
//~ std::cout << "[AMS] "
		//~ << this->name()
		//~ << " @ "
		//~ << this->get_time()
		//~ << ": "
		//~ << in.read()
		//~ << std::endl;
}
