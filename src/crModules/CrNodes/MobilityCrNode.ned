
import crSimulator.core.*;
package crSimulator.crModules.CrNodes;
import crSimulator.crModules.AppLayer.crAppLayer;
import crSimulator.crModules.NetLayer.crNetLayer;
import crSimulator.crModules.MobilityComponent.MobilityComponent;
import crSimulator.crModules.SpectrumSensor.MobilitySpecSensor;


module MobilityCrNode
{
    parameters:
        int address;
        int broadcastAddress;
        string neighbors;
        bool drmEnabled;  // Controls whether the DRM is enabled or not
        bool ceEnabled;   // Controls whether CE is enabled or not
        string appLayer; //= default("crAppLayer");
        string netLayer; //= default("crNetLayer");
        string mobilityMacLayer; //= default("MobilityMacLayer");
        string phyLayer; //= default("crPhyLayer");
        string specSensor; //= default("crSpecSensor");
        string drm; //= default("crDrm");
        string scl; //= default("crScl");
        string ce; //= default("crCognitiveEngine");
        string battery; //= default("crBattery");
        string crstats; //= default("crStats");
        string mobilityComponent; //= default("MobilityComponent");

        @display("bgb=458,348;i=abstract/penguin;bgl=3");

    gates:
        inout ports[];

    submodules:
        appLayer: <appLayer> like IAppLayer {
            @display("p=248,38;i=block/telnet");
        }
        netLayer: <netLayer> like ILayer {
            @display("p=248,139;i=block/network2");
        }
        mobilityMacLayer: <mobilityMacLayer> like IMobilityMacLayer {
            @display("p=248,209;i=old/bwxconn");
        }
        phyLayer: <phyLayer> like IPhyLayer {
            @display("p=248,277;i=block/ifcard");
        }
        DRM: <drm> like IDRM {
            @display("p=51,126;i=abstract/db");
        }
        CE: <ce> like ICognitiveEngine {
            @display("p=49,208;i=abstract/people2");
        }
        SCL: <scl> like ISCL {
            @display("p=141,209;i=block/layer");
        }
        Battery: <battery> like ICrBattery {
            @display("p=51,40;i=block/buffer2");
        }
        crStats: <crstats> like IStats {
            @display("p=141,40;i=device/clock");
        }
        mobilityComponent: MobilityComponent {
            @display("p=387,209");
        }
        mobilitySpecSensor: MobilitySpecSensor {
            @display("p=141,277");
        }
    connections allowunconnected:
        appLayer.ctrlLower <--> netLayer.ctrlUpper;
        appLayer.dataLower <--> netLayer.dataUpper;
        SCL.appInterface <--> appLayer.sclInterface;
        SCL.netInterface <--> netLayer.sclInterface;
        SCL.macInterface <--> mobilityMacLayer.sclInterface;
        SCL.phyInterface <--> phyLayer.sclInterface;
        mobilityMacLayer.dataUpper <--> netLayer.dataLower;
        mobilityMacLayer.ctrlUpper <--> netLayer.ctrlLower;
        phyLayer.dataUpper <--> mobilityMacLayer.dataLower;
        phyLayer.ctrlUpper <--> mobilityMacLayer.ctrlLower;
        SCL.drmInterface <--> DRM.sclInterface;
        SCL.ceInterface <--> CE.sclInterface;
        phyLayer.ssInterface <--> mobilitySpecSensor.phyInterface;
        mobilitySpecSensor.sclInterface <--> SCL.ssInterface;
        mobilitySpecSensor.macInterface <--> mobilityMacLayer.ssInterface;
        mobilitySpecSensor.mobilityInterface <--> mobilityComponent.ssInterface;
        mobilityMacLayer.mobilityInterface <--> mobilityComponent.mobilityInterface;
        mobilityMacLayer.mobilityCtrlInterface <--> mobilityComponent.ctrlInterface;
        for i=0..sizeof(ports)-1 {
            phyLayer.radio++ <--> ports[i];
        }


}
