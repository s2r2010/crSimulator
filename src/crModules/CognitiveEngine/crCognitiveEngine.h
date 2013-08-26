
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An empty box for Cognitive Engine implementation.
 */

#ifndef CRCOGNITIVEENGINE_H_
#define CRCOGNITIVEENGINE_H_

#include "ctrlMsg_m.h"
#include "dataMsg_m.h"

class crCognitiveEngine : public cSimpleModule {

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

};

Define_Module(crCognitiveEngine);

#endif /* CE_H_ */
