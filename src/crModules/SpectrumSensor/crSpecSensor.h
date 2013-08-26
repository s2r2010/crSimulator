
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * This module is responsible to keep track of the channel state and provide this information to any module requesting it. This implementation works with "crMacLayer"
 * and provides it with request sensing results.
 */

#ifndef CRSPECSENSOR_H_
#define CRSPECSENSOR_H_
#include<omnetpp.h>
#include "BaseSpecSensor.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"

class crSpecSensor : public BaseSpecSensor
{
    private:
        int totalChannels, *channelsArray;     // Keeps record of total channels in the RF spectrum and their results
        int patternIndex;                       // Keeps a record of sensing pattern i.e. continuous, rendezvous, patterned!
        double sensingDuration;                 // How much time does sensing take per channel.
        int myAddress, sensedChannel, sensingIteration, currentDataChannel, proposedChannel;
        timerMsg *freeSenseTimer, *dataSenseTimer, *proposedSenseTimer;
        // Stats
        simsignal_t sensingSignal;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void channelState(int, int);
        void senseChannel(int);
        void scheduleSensing(int);
};

Define_Module(crSpecSensor);

#endif /* CRSPECSENSOR_H_ */
