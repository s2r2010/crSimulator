
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * To be extended.. The sole purpose of SCL is to provide a connection among all the different components of the CR node architecture. It does not treat data itself.
 */

#ifndef CRSCL_H_
#define CRSCL_H_


#include "BaseScl.h"
#include "dataMsg_m.h"
#include "ctrlMsg_m.h"
#include "timerMsg_m.h"

class crScl : public BaseScl {

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

};

Define_Module(crScl);

#endif /* SCL_H_ */
