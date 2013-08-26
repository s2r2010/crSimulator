

package crSimulator.crModules.CrNetworks;
import crSimulator.crModules.RFSpectrum.datarateSpectrum;
import ned.DelayChannel;
import crSimulator.core.*;
import crSimulator.crModules.CrNodes.*;
import crSimulator.crModules.PuNodes.*;

network crNetworkDemo extends baseCrNetwork
{
    @display("bgb=510,352,white;i=block/network2");
    submodules:
        CR1: BaseCrNode {
            @display("p=116,235;i=device/wifilaptop");
            address = 1;
            neighbors = "2";
        }
        CR2: BaseCrNode {
            @display("p=355,235;i=device/wifilaptop");
            address = 2;
            neighbors = "1";
        }
        puNode: BasePuNode {
            @display("p=236,90");
        }
    connections:
        CR2.ports++ <--> datarateSpectrum <--> CR1.ports++;
        puNode.ports++ <--> datarateSpectrum <--> CR1.ports++;
        puNode.ports++ <--> datarateSpectrum <--> CR2.ports++;
}

