
#include "datarateSpectrum.h"

datarateSpectrum::datarateSpectrum() {
}
datarateSpectrum::~datarateSpectrum() {
}


bool datarateSpectrum::isTransmissionChannel() const
{
   //, isTransmissionChannel(), determines whether the channel is a trans
    //mission channel, i.e. one that models transmission duration. A transmission channel sets the
    //duration ï¬�eld of packets sent through it (see the setDuration() ï¬�eld of cPacket).
   return true;
}

simtime_t datarateSpectrum::getTransmissionFinishTime() const
{
    // The getTransmissionFinishTime() function is only used with transmission channels, and
    //it should return the simulation time the sender will ï¬�nish (or has ï¬�nished) transmitting. This
    //method is called by modules that send on a transmission channel to ï¬�nd out when the chan-
    //nel becomes available.  The channelâ€™s isBusy() method is implemented simply as return
    //getTransmissionFinishTime()  <  simTime().  For non-transmission channels, the get-
    //TransmissionFinishTime() return value may be any simulation time which is less than or
    //equal to the current simulation time.

    return 0;
}

void datarateSpectrum::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
    //The methodâ€™s arguments are the message object, the simulation time the beginning
    //of the message will reach the channel (i.e. the sum of all previous propagation delays), and a
    //struct in which the method can return the results.
    //The result_t struct is an inner type of cChannel, and looks like this:

    //    struct  result_t  {
    //    simtime_t  delay; //  propagation  delay
    //    simtime_t  duration;    //  transmission  duration
    //    bool  discard; //  whether  the  channel  has  lost  the  message
    //    };

    //The method should model the transmission of the given message starting at the given t time,
    //and store the results (propagation delay, transmission duration, deletion ï¬‚ag) in the result
    //object.  Only the relevant ï¬�elds in the result object need to be changed, others can be left
    //untouched.

    if (this->getDatarate()!=0 && msg->isPacket()) {
    simtime_t duration = ((cPacket *)msg)->getBitLength() / this->getDatarate();
    result.duration = duration;
    }
}
