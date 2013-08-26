

import crSimulator.core.*;
package crSimulator.crModules.Statistics;

simple crStats extends BaseStats like IStats
{
    parameters:
        // App Layer stats

        // MAC layer stats
        //@statistic[rtsSignal](title="RTS Sent"; source="rtsSignal"; record=count);
        //@statistic[rtsFailSignal](title="RTS Failure"; source="rtsFailSignal"; record=count);
        //@statistic[nackSignal](title="Nack Received"; source="nackSignal"; record=count);
        @display("i=device/clock");
        
        @class(crStats);
}

