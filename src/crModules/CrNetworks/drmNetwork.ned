

package crSimulator.crModules.CrNetworks;
import crSimulator.crModules.RFSpectrum.datarateSpectrum;
import ned.DelayChannel;
import crSimulator.core.*;
import crSimulator.crModules.CrNodes.*;
import crSimulator.crModules.PuNodes.*;

network drmNetwork extends baseCrNetwork
{
    @display("bgb=746,404,white;i=block/network2");
    submodules:
        CR1: drmCrNode {
            address = 1;
            neighbors = "2";
            @display("p=441,182");
        }
        CR2: drmCrNode {
            address = 2;
            neighbors = "1";
            @display("p=279,182");
        }
        gsm: GSM {
            @display("p=543,77");
        }
        ism: ISM {
            @display("p=172,48");
        }
    connections:
        CR2.ports++ <--> datarateSpectrum <--> CR1.ports++;
        gsm.ports++ <--> datarateSpectrum <--> CR1.ports++;
        ism.ports++ <--> datarateSpectrum <--> CR2.ports++;
}

