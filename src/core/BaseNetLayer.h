

#ifndef BASENETLAYER_H_
#define BASENETLAYER_H_

#include <omnetpp.h>
#include <Logging.h>

class BaseNetLayer : public cSimpleModule
{
    public:
        BaseNetLayer();
        virtual ~BaseNetLayer();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(BaseNetLayer);

#endif /* BASENETLAYER_H_ */
