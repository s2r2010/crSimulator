
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * It is intended to produce PU activity patterns that match the real observed activities for GSM band.
 */

#ifndef PUGSM_H_
#define PUGSM_H_
#include "dataMsg_m.h"
#include "timerMsg_m.h"
#include "Logging.h"

class puGSM : public cSimpleModule{
public:
    puGSM();
    virtual ~puGSM();
protected:
    virtual void handleMessage(cMessage *msg);
    virtual void initialize();
    void setTimer();
    void broadcast(dataMsg *msg);
    void scheduleEot();

private:
    void updateGUI(double time, int chID);
    cMessage *apptimer, *eot;
    std::map<int, std::string> colorMap;
    std::string puState;
    std::string puColor;
    int totalChannels, puChannel;
    double idleDuration, busyDuration; //arrivalRate, txDuration;
};

Define_Module(puGSM);

#endif /* PUGSM_H_ */
