

#include "mysqlDRM.h"

void mysqlDRM::initialize()
{
    // CREATE YOUR OWN SENSING TABLE IN DRM DATABASE
    // IF the table exists, remove old data or not?
    myAddress = getParentModule()->par("address");
    connectDatabase();
    createSensingTable();
}
void mysqlDRM::handleMessage(cMessage *msg)
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
void mysqlDRM::storeData(ctrlMsg *msg)
{
    sprintf(myName, "CR%d", myAddress);
    for (unsigned int x =0; x<msg->getSensingResultArraySize(); x++){
        sprintf(Query, "INSERT INTO drm.%s values(%d, %d)", myName, x+1, msg->getSensingResult(x));
        mysql_query(connection, Query);
        ev<< "The Query is "<< Query<< endl;
    }
    delete msg;
}
void mysqlDRM::connectDatabase()
{
    mysql_init(&mysql);
    connection = mysql_real_connect(&mysql,"localhost","drmuser","test","drm",0,0,0);
    if (connection == NULL)
    {
        EV<< mysql_error(&mysql) << endl;
    }
    else
    {
        EV<< "Connection to DRM database successful"<<endl;
    }
}
void mysqlDRM::createSensingTable()
{
    sprintf(myName, "CR%d", myAddress);
    sprintf(Query, "CREATE TABLE IF NOT EXISTS %s (chID INT, value INT)", myName);
    mysql_query(connection, Query);
    //sprintf(myName, "CR%davg", myAddress);
    //sprintf(Query, "CREATE TABLE IF NOT EXISTS %s (chID INT, avg INT)", myName);
    //mysql_query(connection, Query);

    bool newData = par("newData");
    if(newData == true)
    {
        sprintf(Query, "TRUNCATE TABLE %s", myName);
        mysql_query(connection, Query);
    }
}


void mysqlDRM::finish()
{
    if (connection != NULL)
    {
        mysql_close(connection);
    }
}
