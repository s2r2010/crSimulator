

#ifndef DATARATESPECTRUM_H_
#define DATARATESPECTRUM_H_

#include<omnetpp.h>

class datarateSpectrum : public cDatarateChannel{
public:
    datarateSpectrum();
    virtual ~datarateSpectrum();
protected:
        bool isTransmissionChannel() const;
        simtime_t getTransmissionFinishTime() const;
        void processMessage(cMessage *msg, simtime_t t, result_t& result);
};

Define_Channel(datarateSpectrum);

#endif /* DATARATESPECTRUM_H_ */
