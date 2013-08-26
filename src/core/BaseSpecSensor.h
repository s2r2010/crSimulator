

#ifndef BASESPECSENSOR_H_
#define BASESPECSENSOR_H_
#include <omnetpp.h>
class BaseSpecSensor : public cSimpleModule
{
    public:
        BaseSpecSensor();
        virtual ~BaseSpecSensor();
};

Define_Module(BaseSpecSensor);

#endif /* BASESPECSENSOR_H_ */
