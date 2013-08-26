

import crSimulator.core.*;
package crSimulator.crModules.SpectrumSensor;

simple MobilitySpecSensor extends BaseSpecSensor like ISpecSensor
{
    parameters:
        double sensingDuration = default(0.1);  				// How much time does channel sensing take!
        double sensingConsumption = default(0.1); 			// How much energy is consumed by sensing!
        // How many times does the node sweep through all channel with sensing until finding free channel.
        int sensingIterations @signal[sensing](type="int") = default(3);
        int sensingPattern = default(1);  // 1 for continuous, 2 for Patterned.
        @display("i=block/wtx");
        @class(MobilitySpecSensor);
        
        // Statistics
        @signal[sensingSignal](type="int");
    	    @statistic[sensingSignal](title="Spectrum Sensing"; source="sensingSignal"; record=count,count/60,vector,histogram,mean);
        @signal[sensedChannelSignal](type="int");
    	    @statistic[sensedChannelSignal](title="Sensed Channel"; source="sensedChannelSignal"; record=count,count/60,vector,histogram,mean);    	    
    gates:
        inout mobilityInterface;
}
