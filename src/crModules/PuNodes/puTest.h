

#ifndef TESTPU_H_
#define TESTPU_H_
#include"BasePuNode.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "Logging.h"

class puTest : public BasePuNode
{
    private:
        std::map<int, std::string> colorMap;
        cMessage *apptimer, *eot;
        int totalChannels, puChannel;
        double arrivalRate, txDuration;
        std::string puState;
        void updateGUI(double time, int chID);
    protected:
        void setTimer();
        void broadcast(dataMsg *msg);
        void scheduleEot();
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(puTest);

#endif /* PUNODE_H_ */
