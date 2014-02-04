

package crSimulator.crModules.PuNodes;
import crSimulator.core.*;

simple puGSM extends PuNode like IPuNode
{
    parameters:
        @class(puGSM);
}

simple puISM extends PuNode like IPuNode
{
    parameters:
        @class(puISM);
}

simple puTest extends PuNode like IPuNode
{
    parameters:
        @class(puTest);
}

module BasePuNode
{
    parameters:
        int puChannel = default(0);
        bool puEnabled = default(false);
        double arrivalRate = default(0.5);
        double puTxDuration = default(5.0);
        string puType;// = default("testPU");
        int address = default(1);
        @display("i=device/antennatower");

    gates:
        inout ports[];

    submodules:
        appLayer: <puType> like IPuNode;
    connections:
        for i=0..sizeof(ports)-1 {
            appLayer.radio++ <--> ports[i];
        }
}
