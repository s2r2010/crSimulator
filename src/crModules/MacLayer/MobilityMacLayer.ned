
import crSimulator.core.*;
package crSimulator.crModules.MacLayer;

simple MobilityMacLayer extends BaseMobilityMacLayer like IMobilityMacLayer
{
    parameters:
        int rtsAttempts = default(1);   // How many attempts can be made for RTS
        int totalFrames = default(intuniform(20, 50));  // Represents the amount of data that needs to be sent
        bool AckEnabled = default(false);   // Should all packet reception be acknowledged or not?
        bool sensePerPacket = default(true);  // The transmission channel is sensed for each packet or only at the begging 
        bool drmEnabled = default(false);

        @signal[rtsSignal](type="int");
            @statistic[rtsSignal](title="RTS Attempts"; source="rtsSignal"; record=count,count/60,vector,mean);
        @signal[txDataSignal](type="int");
            @statistic[txDataSignal](title="TX Data"; source="txDataSignal"; record=count,count/60,vector,mean);
        @signal[txAckSignal](type="int");
            @statistic[txAckSignal](title="TX ACK"; source="txAckSignal"; record=count,count/60,vector,mean);
        @signal[rxDataSignal](type="int");
            @statistic[rxDataSignal](title="RX Data"; source="rxDataSignal"; record=count,count/60,vector,mean);
        @signal[rxAckSignal](type="int");
            @statistic[rxAckSignal](title="RX ACK"; source="rxAckSignal"; record=count,count/60,vector,mean);
        @signal[nackSignal](type="int");
            @statistic[nackSignal](title="NACK"; source="nackSignal"; record=count,count/60,vector,mean);
        @signal[handover](type="int");
            @statistic[handover](title="Handovers"; source="handover"; record=count,count/60,vector,mean);
        @signal[txMobilitySignal](type="int");
            @statistic[txMobilitySignal](title="TX Mobility"; source="txMobilitySignal"; record=count,count/60,vector,mean);
        @signal[rxMobilitySignal](type="int");
            @statistic[rxMobilitySignal](title="RX Mobility"; source="rxMobilitySignal"; record=count,count/60,vector,mean);
        @signal[totalTxPacketsSignal](type="int");
            @statistic[totalTxPacketsSignal](title="Total TX Packets"; source="totalTxPacketsSignal"; record=count,count/60,vector,mean);
        @signal[totalRxPacketsSignal](type="int");
            @statistic[totalRxPacketsSignal](title="Total RX Packets"; source="totalRxPacketsSignal"; record=count,count/60,vector,mean); 
           
        @display("i=block/mac");
        @class(MobilityMacLayer);
}

        

