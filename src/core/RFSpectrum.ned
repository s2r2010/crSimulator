

package crSimulator.core;

import ned.DatarateChannel;
import ned.DelayChannel;

channel RFSpectrum extends DatarateChannel
{
    @class("suChannel"); // TEMPORARY
    @display("ls=dodgerBlue,2,da");
    datarate = 6Mbps;
}

channel delayspectrum extends DelayChannel
{
    @class("spectrum");
    @display("ls=dodgerBlue,2,da");
    delay = 5ms;
}
