
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * The Phy layer receives messages from all connections/channels. Any physical layer parameters need to be appended to the outgoing messages.
 */

#include "crPhyLayer.h"

void crPhyLayer::initialize()
{
    myAddress = getParentModule()->par("address");
    // Statistics
    //sensingSignal = registerSignal("sensing");
    //handoverSignal = registerSignal("handover");
}

void crPhyLayer::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("ctrlUpper$i")) // Ctrl msg from MAC
    {
        //
    }
    else if(msg->arrivedOn("dataUpper$i")) // Data msg from MAC
    {
        // send data out.
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        broadcast(recMsg);
    }
    else if(msg->arrivedOn("ssInterface$i")) // Spectrum sensing interface msg
    {
        //
    }
    else // msg from outside world
    {
        // msg for me!
        // msg for others!
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        dataMsg *copy = (dataMsg *) recMsg->dup();
        send(recMsg, "dataUpper$o");
        send(copy, "ssInterface$o");
    }
}

void crPhyLayer::broadcast(dataMsg *msg)
{
        for ( int x=0; x<gateSize("radio"); x++)
        {
            dataMsg *copy = (dataMsg *) msg->dup();
            send(copy, "radio$o", x);
        }
        delete msg;
}



