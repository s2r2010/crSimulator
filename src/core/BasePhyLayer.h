

#ifndef BASEPHYLAYER_H_
#define BASEPHYLAYER_H_

#include <omnetpp.h>
class BasePhyLayer : public cSimpleModule
{
    public:
        BasePhyLayer();
        virtual ~BasePhyLayer();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(BasePhyLayer);

#endif /* BASEPHYLAYER_H_ */
