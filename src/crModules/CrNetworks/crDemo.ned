

package crSimulator.crModules.CrNetworks;
import crSimulator.crModules.RFSpectrum.datarateSpectrum;
import ned.DelayChannel;
import crSimulator.core.*;
import crSimulator.crModules.CrNodes.*;
import crSimulator.crModules.PuNodes.*;

network crDemo extends baseCrNetwork
{
    @display("bgb=576,438,white;i=block/network2");
    submodules:
        CR1: BaseCrNode {
            @display("p=102,218;i=device/wifilaptop");
            address = 1;
            neighbors = "2";
        }
        CR2: BaseCrNode {
            @display("p=473,218;i=device/wifilaptop");
            address = 2;
            neighbors = "1";
        }
        gsm1: GSM {
            @display("p=281,92");
        }
        gsm2: GSM {
            @display("p=287,342");
        }
    connections:
        CR2.ports++ <--> datarateSpectrum <--> CR1.ports++;
        gsm1.ports++ <--> datarateSpectrum <--> CR1.ports++;
        gsm1.ports++ <--> datarateSpectrum <--> CR2.ports++;
        gsm2.ports++ <--> datarateSpectrum <--> CR2.ports++;
        gsm2.ports++ <--> datarateSpectrum <--> CR1.ports++;
}

