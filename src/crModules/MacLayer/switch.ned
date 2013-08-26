
import crSimulator.core.*;
package crSimulator.crModules.MacLayer;

simple switchMac extends BaseMacLayer like ILayer
{
    parameters:
        int rtsAttempts = default(1);   // How many attempts can be made for RTS
        int totalFrames = default(intuniform(20, 50));  // Represents the amount of data that needs to be sent
        bool AckEnabled = default(false);   // Should all packet reception be acknowledged or not?
        bool sensePerPacket = default(true);  // The transmission channel is sensed for each packet or only at the begging 
        bool drmEnabled = default(false);

        @signal[rtsSignal](type="int");
            @statistic[rtsSignal](title="RTS Attempts"; source="rtsSignal"; record=count);
        @signal[dataSignal](type="int");
            @statistic[dataSignal](title="Data Received"; source="dataSignal"; record=count);
        @signal[nackSignal](type="int");
            @statistic[nackSignal](title="NACK"; source="nackSignal"; record=count);
        @signal[handover](type="int");
            @statistic[handover](title="Handovers"; source="handover"; record=count);
        @display("i=block/mac");
        @class(crMacLayer);
}