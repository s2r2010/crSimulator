
// SHAH NAWAZ KHAN
// Technische Universitat Ilmenau, Deutschland
// shazmailz@gmail.com, shah-nawaz.khan@tu-ilmenau.de

package crSimulator.core;

moduleinterface ILayer						// Provides a base layer template
{
    gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout sclInterface @loose;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Application Layer   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IAppLayer					// Provide a template for Application layer with no links to any higher layer
{
    gates:
        inout dataLower;
        inout ctrlLower;
        inout sclInterface @loose;
}
simple BaseAppLayer like IAppLayer
{
    gates:
        inout dataLower;
        inout ctrlLower;
        inout sclInterface;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Network Layer   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
simple BaseNetLayer like ILayer
{
    @display("i=block/network2");
        gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout sclInterface @loose;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SDR Controller  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
simple BaseController like ILayer
{
    gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout sclInterface @loose;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MAC Layer  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IMacLayer
{
        gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout sclInterface @loose;
        inout ssInterface @loose;
}
simple BaseMacLayer like IMacLayer
{
        gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout sclInterface @loose;
        inout ssInterface @loose;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Physical Layer >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IPhyLayer	// Similar to other layers except that it has an array of ports
{
    gates:
        inout ctrlUpper;
        inout dataUpper;
        inout sclInterface @loose;
        inout ssInterface @loose;
        inout radio[];    
}
simple BasePhyLayer like IPhyLayer
{
    gates:
        inout ctrlUpper;
        inout dataUpper;
        inout sclInterface @loose;
        inout ssInterface @loose;
        inout radio[];   
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Signalling & Communication Link >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface ISCL						// Template for Signalling and Communication Link. Used for interfacing other components to protocol stack layers.
{
    gates:
        inout appInterface;
        inout netInterface;
        inout macInterface;
        inout phyInterface;
        inout drmInterface;
        inout ceInterface;
        inout ssInterface; //spectrum sensor
}
simple BaseScl like ISCL
{
    gates:
        inout appInterface;
        inout netInterface;
        inout macInterface;
        inout phyInterface;
        inout drmInterface;
        inout ceInterface;
        inout ssInterface; //spectrum sensor
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Distributed Resource Map >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IDRM						// Template for Distributed Resource Map. Has to be implemented separately
{
    gates:
        inout sclInterface;
}
simple BaseDrm like IDRM
{
    gates:
        inout sclInterface;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Cognitive Engine  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface ICognitiveEngine			// Template for Cogntive Engine
{
    gates:
        inout sclInterface;
}
simple BaseCognitiveEngine like ICognitiveEngine
{
    gates:
        inout sclInterface;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  Spectrum Sensor >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface ISpecSensor
{
    gates:
        inout phyInterface;
        inout macInterface;
        inout sclInterface;
}
simple BaseSpecSensor like ISpecSensor
{
        gates:
        inout phyInterface;
        inout macInterface;
        inout sclInterface;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  Battery Indicator  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface ICrBattery
{
}
simple BaseCrBattery like ICrBattery
{
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Stats Collection >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IStats
{
}
simple BaseStats like IStats
{
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Base Primary User >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IPuNode	
{
    gates:
        inout radio[];    
}

simple PuNode like IPuNode
{
    gates:
        inout radio[];   
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Mobility Component >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IMobilityComponent
{
    gates:
        inout mobilityInterface @loose;
        inout sclInterface @loose;
        inout ssInterface @loose;
        inout ctrlInterface @loose;
}
simple BaseMobilityComponent like IMobilityComponent
{
    gates:
        inout mobilityInterface;
        inout sclInterface;
        inout ssInterface;
        inout ctrlInterface;       
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Mobility MAC Layer  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
moduleinterface IMobilityMacLayer
{
    gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout mobilityInterface @loose;
        inout mobilityCtrlInterface @loose;        
        inout sclInterface @loose;
        inout ssInterface @loose;
}
simple BaseMobilityMacLayer like IMobilityMacLayer
{
    gates:
        inout ctrlUpper;
        inout ctrlLower;
        inout dataUpper;
        inout dataLower;
        inout mobilityInterface;
        inout mobilityCtrlInterface;         
        inout sclInterface;
        inout ssInterface;
}
