

#ifndef SQLITEDRM_H_
#define SQLITEDRM_H_
#include <omnetpp.h>
#include <string.h>
#include <sqlite3.h>
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"

class sqliteDRM : public cSimpleModule{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    void connectDatabase();                     // Connects to DRM database
    void createSensingTable();                  // Creates a node specific table in DRM Database
    void storeData(ctrlMsg *msg);               // Stores the sensing data sent by spectrum sensor

    sqlite3 *db;

    char Query[256], myName[25];
    int myAddress;
};

Define_Module(sqliteDRM);

#endif /* SQLITEDRM_H_ */
