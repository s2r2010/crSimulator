

#include "sqliteDRM.h"

void sqliteDRM::initialize()
{
    db = NULL;
    myAddress = getParentModule()->par("address");
    connectDatabase();
    createSensingTable();
}
void sqliteDRM::handleMessage(cMessage *msg)
{
    ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
    switch(recMsg->getKind())
    {
    case SensingRESULT:
        // Store in the database
        storeData(recMsg);
        break;
    default:
        ev<<"DRM: Unknown message type received from SCL"<<endl;
        delete recMsg;
        break;
    }
}
void sqliteDRM::storeData(ctrlMsg *msg)
{
    sprintf(myName, "CR%d", myAddress);
    for (unsigned int x =0; x<msg->getSensingResultArraySize(); x++){
        sprintf(Query, "INSERT INTO %s values(%d, %d)", myName, x+1, msg->getSensingResult(x));
        sqlite3_exec(db, Query, 0, 0, 0);
        ev<< "The Query is "<< Query<< endl;
    }
    delete msg;
}
void sqliteDRM::connectDatabase()
{
    int rc = sqlite3_open("/home/shah/drm.db", &db);
    if (rc)
    {
        ev<< "Failed to Open DRM database\n";
    }
    else
        ev<< "DRM Database Opened\n";

}
void sqliteDRM::createSensingTable()
{
    sprintf(myName, "CR%d", myAddress);
    sprintf(Query, "CREATE TABLE IF NOT EXISTS %s (chID INT, value INT)", myName);
    sqlite3_exec(db, Query, 0, 0, 0);
    bool newData = par("newData");
    if(newData == true)
    {
        sprintf(Query, "DELETE FROM %s", myName);
        sqlite3_exec(db, Query, 0, 0, 0);
    }
}

void sqliteDRM::finish()
{
    sqlite3_close(db);
    db = NULL;
}


