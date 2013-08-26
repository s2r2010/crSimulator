
import crSimulator.core.*;
package crSimulator.crModules.CrNodes;

/// CR node based on Component Architecture of TU Ilmenau.

module BaseCrNode
{
    parameters:
        int address;
        string neighbors;
        bool drmEnabled;  // Controls whether the DRM is enabled or not
        bool ceEnabled;   // Controls whether CE is enabled or not
        string appLayer; //= default("crAppLayer");
        string netLayer; //= default("crNetLayer");
        string macLayer; //= default("crMacLayer");
        string phyLayer; //= default("crPhyLayer");
        string specSensor; //= default("crSpecSensor");
        string drm; //= default("crDrm");
        string scl; //= default("crScl");
        string ce; //= default("crCognitiveEngine");
        string battery; //= default("crBattery");
        string crstats; //= default("crStats");

        @display("bgb=304,348;i=device/palm2");
    gates:
        inout ports[];

    submodules:
        appLayer: <appLayer> like IAppLayer {
            @display("p=228,40;i=block/telnet");
        }
        netLayer: <netLayer> like ILayer {
            @display("p=228,134;i=block/network2");
        }
        macLayer: <macLayer> like IMacLayer {
            @display("p=228,209;i=old/bwxconn");
        }
        phyLayer: <phyLayer> like IPhyLayer {
            @display("p=228,277;i=block/ifcard");
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
        specSensor: <specSensor> like ISpecSensor {
            @display("p=141,277;i=block/wtx");
        }
        crStats: <crstats> like IStats {
            @display("p=141,40;i=device/clock");
        }
    connections allowunconnected:
        appLayer.ctrlLower <--> netLayer.ctrlUpper;
        appLayer.dataLower <--> netLayer.dataUpper;
        SCL.appInterface <--> appLayer.sclInterface;
        SCL.netInterface <--> netLayer.sclInterface;
        SCL.macInterface <--> macLayer.sclInterface;
        SCL.phyInterface <--> phyLayer.sclInterface;
        macLayer.dataUpper <--> netLayer.dataLower;
        macLayer.ctrlUpper <--> netLayer.ctrlLower;
        phyLayer.dataUpper <--> macLayer.dataLower;
        phyLayer.ctrlUpper <--> macLayer.ctrlLower;
        SCL.drmInterface <--> DRM.sclInterface;
        SCL.ceInterface <--> CE.sclInterface;
        phyLayer.ssInterface <--> specSensor.phyInterface;
        specSensor.sclInterface <--> SCL.ssInterface;
        specSensor.macInterface <--> macLayer.ssInterface;
        for i=0..sizeof(ports)-1 {
            phyLayer.radio++ <--> ports[i];
        }
}
