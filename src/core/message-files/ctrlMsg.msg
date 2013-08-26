
enum CtrlType {
AppREQUEST = 1;             // Request to send data from application layer
AppCONFIRMATION = 2;        // Confirmation to send data from network/mac layer
SenseFreeCHANNEL = 3;       // MAC Request for any available free channel to Spectrum Sensor
SenseDataCHANNEL = 4;       // MAC Request for sensing the data channel
SenseProposedCHANNEL = 5;   // MAC request for confirming the state of a proposed channel from transmitter.
SenseFreeREPLY = 6;         // Reply from spectrum sensor about free channel request
SenseDataREPLY = 7;         // Reply from spectrum sensor about data channel state
SenseProposedREPLY = 8;
TxSUCCESS = 9;              // MAC Informs APP about successful transmission
TxFAIL = 10;                    // MAC informs APP about failed tx
SensingRESULT = 11;         // Spectrum Sensing publishes Sensing results on SCL
SenseDrmCHANNEL = 12;
SenseDrmREPLY = 13;
SenseChannelMap = 14;
SenseChannelMapReply = 15;
RendezvousSuccess = 16;
RendezvousFail = 17;
};

message ctrlMsg
{
    int kind @enum(CtrlType);
    int numberOfPackets;
    int destination;
    int channelID;
    bool channelState;      // true means data channel is still free.
    int SensingChannel;     // the id of the channel that needs to be sensed
    int sensingResult[];        // Results of spectrum sensing for publishing on SCL.
    int channelMap[];
}
