

#ifndef BASEAPPLAYER_H_
#define BASEAPPLAYER_H_

#include <omnetpp.h>
#include <Logging.h>

class BaseAppLayer : public cSimpleModule
{
    public:
        BaseAppLayer();
        virtual ~BaseAppLayer();
};

Register_Class(BaseAppLayer);

#endif /* BASEAPPLAYER_H_ */
