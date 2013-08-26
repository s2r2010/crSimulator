

#ifndef MYSQLDRM_H_
#define MYSQLDRM_H_

#include <omnetpp.h>
#include <string.h>
#include <mysql.h>
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"

class mysqlDRM : public cSimpleModule {
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    void connectDatabase();                     // Connects to DRM database
    void createSensingTable();                  // Creates a node specific table in DRM Database
    void storeData(ctrlMsg *msg);               // Stores the sensing data sent by spectrum sensor

    MYSQL *connection, mysql;
    char Query[256], myName[25];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int myAddress;

};

Define_Module(mysqlDRM);

#endif /* DRM_H_ */
