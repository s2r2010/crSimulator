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

import crSimulator.core.*;
package crSimulator.crModules.Battery;

simple crBattery extends BaseCrBattery like ICrBattery
{
    parameters:
        int size = default(100);
        @class(crBattery);
        @display("i=block/buffer2");
    gates:
        input in @loose; // allow to remain unconnected
}