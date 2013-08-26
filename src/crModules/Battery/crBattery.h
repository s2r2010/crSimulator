
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * To be used only when energy efficiency related metrics are needed. Should be implemented through the "Signals" concept on OMNeT++
 */

#ifndef CRBATTERY_H_
#define CRBATTERY_H_

#include "BaseCrBattery.h"

class crBattery : public BaseCrBattery{
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(crBattery);

#endif /* BATTERY_H_ */
