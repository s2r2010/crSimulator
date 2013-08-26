
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * The network layer only selects a random destination node among its one-hop peer nodes in the network. The addresses of the one-hop neighbors need to be
 * provided in the topology (.NED) file of the network
 */


#include "crNetLayer.h"

void crNetLayer::initialize()
{
    // get the neighbors list
    const char *dist = getParentModule()->par("neighbors");
    cStringTokenizer tokenizer(dist);
    while (tokenizer.hasMoreTokens())
        neighbors.push_back(atoi(tokenizer.nextToken()));
    destinationNode = 0;
}

void crNetLayer::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("ctrlUpper$i"))
    {
        send(msg, "ctrlLower$o");
    }
    else if (msg->arrivedOn("ctrlLower$i"))
    {
        send(msg, "ctrlUpper$o");
    }
    else if (msg->arrivedOn("dataUpper$i"))
    {
        send(msg, "dataLower$o");
    }
    else if (msg->arrivedOn("dataLower$i"))
    {
        send(msg, "dataUpper$o");
    }
    else
    {
        delete msg;
        LOG(" bamboozled at network layer");
    }
}
