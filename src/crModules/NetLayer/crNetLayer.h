
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * The network layer only selects a random destination node among its one-hop peer nodes in the network. The addresses of the one-hop neighbors need to be
 * provided in the topology (.NED) file of the network
 */

#ifndef CRNETLAYER_H_
#define CRNETLAYER_H_

#include "BaseNetLayer.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"

class crNetLayer : public BaseNetLayer {
private:
    std::vector<int> neighbors; // holds result
    int destinationNode;        // holds destination address.
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(crNetLayer);

#endif /* NETWORKLAYER_H_ */
