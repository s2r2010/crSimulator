#ifndef BASEMOBILITYCOMPONENT_H_
#define BASEMOBILITYCOMPONENT_H_

#include <omnetpp.h>
#include <Logging.h>

class BaseMobilityComponent : public cSimpleModule
{
    public:
        BaseMobilityComponent();
        virtual ~BaseMobilityComponent();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(BaseMobilityComponent);

#endif /* BASEMOBILITYCOMPONENT_H_ */
