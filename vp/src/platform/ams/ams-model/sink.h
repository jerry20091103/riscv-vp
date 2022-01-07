#ifndef SINK_H
#define SINK_H
#include <systemc-ams>

SCA_TDF_MODULE(sink)
{
sca_tdf::sca_in<double> in;
SCA_CTOR(sink)
: in("in")
{}
void processing();
};

#endif // SINK_H
