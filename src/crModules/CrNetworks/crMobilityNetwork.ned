

package crSimulator.crModules.CrNetworks;
import crSimulator.crModules.RFSpectrum.datarateSpectrum;
import ned.DelayChannel;
import crSimulator.core.*;
import crSimulator.crModules.CrNodes.*;
import crSimulator.crModules.PuNodes.*;

network crMobilityNetwork extends baseCrNetwork
{
    @display("bgb=591,324,white;i=block/network2");
    submodules:
        CR1: MobilityCrNode {
            address = 1;
            broadcastAddress = -1;
            neighbors = "2";
            @display("p=370,157");
        }
        CR2: MobilityCrNode {
            address = 2;
            broadcastAddress = -1;            
            neighbors = "1";
            @display("p=208,157");
        }
        pu1: BasePuNode {
            address = 3;
            puChannel = intuniform(0,3);            
            @display("p=292,232");
        }
        pu2: BasePuNode {
            address = 4;
            puChannel = intuniform(0,3);            
            @display("p=292,47");
        }
        pu3: BasePuNode {
            address = 5;
            puChannel = intuniform(0,3);
            @display("p=64,157");
        }
    connections:
        CR2.ports++ <--> datarateSpectrum <--> CR1.ports++;
        CR1.ports++ <--> datarateSpectrum <--> pu1.ports++;
        CR2.ports++ <--> datarateSpectrum <--> pu1.ports++;
        CR1.ports++ <--> datarateSpectrum <--> pu2.ports++;
        CR2.ports++ <--> datarateSpectrum <--> pu2.ports++;
        CR2.ports++ <--> datarateSpectrum <--> pu3.ports++;
}

