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

#ifndef BASEMACLAYER_H_
#define BASEMACLAYER_H_

#include <omnetpp.h>
#include <Logging.h>
class BaseMacLayer : public cSimpleModule
{
    public:
        BaseMacLayer();
        virtual ~BaseMacLayer();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(BaseMacLayer);

#endif /* BASEMACLAYER_H_ */
