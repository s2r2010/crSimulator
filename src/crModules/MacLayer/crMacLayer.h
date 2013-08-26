
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An RTS/CTS based Mac implementation that is connected with Spectrum Sensing Module and can avoid Primary Users by switching channels. Channel switching/mobility
 * is random.
 */

#ifndef CRMACLAYER_H_
#define CRMACLAYER_H_

#include "BaseMacLayer.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"

class crMacLayer : public BaseMacLayer {

private:
        int numberOfPackets, totalFrames;  // The number of packets to send in response to Application request.
        bool perPacketSensing;   // Should the transmission channel be sensed per packet or per session
        bool drmEnabled;
        bool ackEnabled;


    int myAddress;  // MAC address of node
    int currentDestination;  // Keeps track on next hop or destination
    int currentDataChannel;    // The channel currently used for data transmission.
    int proposedChannel;
    bool rtsSent;         // to keep track of sent RTS
    timerMsg *rtsTimer, *ackTimer;   // RTS timer for next attempt and ACK timeout
    dataMsg *frame;       // keeps a copy of sent packet for retransmission
    bool isReceiving, isTransmitting;   // keeps track of radio state
    int rtsAttempts, frameSendAttempts, ctsDestination; // keeps track of number of retransmissions.

    // Statistics
    simsignal_t rtsSignal, dataSignal;
    simsignal_t handover;
    simsignal_t nackSignal;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // Msg sending functions
    void sendRTS(int);
    void sendCTS();
    void sendData();
    void sendAck();
    void sendNack();

    // Msg handling functions
    void handleRTS(dataMsg *msg);
    void handleCTS(dataMsg *msg);
    void handleData(dataMsg *msg);
    void handleAck();
    void handleNack();
    void handleEOT();
    void handlePU();

    // Timer Setting/Clearing functions
    void setRTStimer();
    void clearRTStimer();
    void setAckTimeOut();

    // Objective functions
    void senseRequest(int);
    void getIdle();
};

Define_Module(crMacLayer);

#endif /* MACLAYER_H_ */
