//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package crSimulator.crModules.CrNetworks;
import crSimulator.crModules.CrNodes.BaseCrNode;
import crSimulator.crModules.PuNodes.GSM;
import crSimulator.crModules.RFSpectrum.datarateSpectrum;


//
// TODO documentation
//
network testNet
{
    @display("bgb=500,298");
    submodules:
        gsm: GSM {
            @display("p=228,53");
        }
        gsm1: GSM {
            @display("p=219,245");
        }
        baseCrNode: BaseCrNode {
            @display("p=63,142");
        }
        baseCrNode1: BaseCrNode {
            @display("p=357,143");
        }
    connections:
        baseCrNode.ports++ <--> datarateSpectrum <--> gsm.ports++;
        gsm.ports++ <--> datarateSpectrum <--> baseCrNode1.ports++;
        baseCrNode1.ports++ <--> datarateSpectrum <--> gsm1.ports++;
        gsm1.ports++ <--> datarateSpectrum <--> baseCrNode.ports++;
        baseCrNode.ports++ <--> datarateSpectrum <--> baseCrNode1.ports++;
}

