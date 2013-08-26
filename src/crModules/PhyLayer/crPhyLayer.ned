

import crSimulator.core.*;
package crSimulator.crModules.PhyLayer;


simple crPhyLayer extends BasePhyLayer like IPhyLayer
{
    parameters:
        //@statistic[sensing](title="Spectrum Sensing"; source="sensing"; record=count,vector);
        @display("i=block/ifcard");
        //@signal[handoverSignal](type="int");
        //@statistic[handoverSignal](title="Handover"; source="handoverSignal"; record=count);
        @class(crPhyLayer);
}