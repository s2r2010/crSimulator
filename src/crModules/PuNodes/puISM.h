

#ifndef PUNODE_H_
#define PUNODE_H_
#include"BasePuNode.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "Logging.h"

class puISM : public BasePuNode
{
    private:
        cMessage *apptimer, *waitTimer, *eot;
        int puChannel, totalChannels;
        double arrivalRate;
    protected:
        void setTimer();
        void broadcast(dataMsg *msg);
        void scheduleEot();
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(puISM);

#endif /* PUNODE_H_ */
