
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * This module is responsible to keep track of the channel state and provide this information to any module requesting it. This implementation works with "drmMacLayer" and with
 * the DRM module. It provides regular spectrum sensing updates to the DRM module so that DRM can aggregate them over time and characterize the channels accordingly
 */

#ifndef MOBILITYSPECSENSOR_H_
#define MOBILITYSPECSENSOR_H_
#include "BaseSpecSensor.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"
#include "Logging.h"

class MobilitySpecSensor : public BaseSpecSensor{
private:
        int totalChannels, *channelsArray;     // Keeps record of total channels in the RF spectrum and their results
        //int patternIndex;                       // Keeps a record of sensing pattern i.e. continuous, rendezvous, patterned!
        double sensingDuration;                 // How much time does sensing take per channel.
        int myAddress, sensedChannel, sensingIteration, currentDataChannel, proposedChannel, drmChannel;
        int broadcastAddress;
        timerMsg *freeSenseTimer, *dataSenseTimer, *proposedSenseTimer, *drmSenseTimer, *mobilitySenseTimer;//, *publishTimer;
        simsignal_t sensingSignal, sensedChannelSignal;
        void notifyMACLayer(dataMsg *msg);

    protected:
        virtual void initialize();                  // called at the beginning of simulation
        virtual void init();                        // initializes the parameters
        virtual void handleMessage(cMessage *msg);
        virtual void channelState(int, int);        // Updates the state of channel as free or busy
        void senseChannel(int);                     // Senses a particular channel state based on channel id.
        void scheduleSensing(int);                  // A timer method to simulation the delay of spectrum sensing.
        void publishSensingResults();               // Publishes the results of spectrum sensing on the SCL.
};

Define_Module(MobilitySpecSensor);
#endif /* MOBILITYSPECSENSOR_H_ */
