
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * To be extended.. The sole purpose of SCL is to provide a connection among all the different components of the CR node architecture. It does not treat data itself.
 */

#include "crScl.h"

void crScl::initialize()
{
}

void crScl::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("ssInterface$i")){
        //ctrlMsg *sResult = check_and_cast<ctrlMsg *>(msg);
        //for (int x=0; x<sResult->getSensingResultArraySize(); x++){
            //ev<<"SCL: Channel "<<x<< " is "<<sResult->getSensingResult(x)<< " \n";
        //}
        //delete sResult;
        send(msg, "drmInterface$o");  // Send to DRM.
    }
    else if (msg->arrivedOn("drmInterface$i")){
        send(msg,"macInterface$o");
    }
}
