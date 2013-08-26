import crSimulator.core.*;
package crSimulator.crModules.MobilityComponent;

simple MobilityComponent extends BaseMobilityComponent like IMobilityComponent
{
  parameters:

    double broadcastTimerDuration = default(0.001);
    
    @class(MobilityComponent);
    @display("i=device/cellphone");

    @signal[scanChannelMapRequest](type="int");
        @statistic[scanChannelMapRequest](title="Channel Map Requests"; source="scanChannelMapRequest"; record=count,count/60,vector);
    @signal[scanChannelMapReply](type="int");
        @statistic[scanChannelMapReply](title="Channel Map Replies"; source="scanChannelMapReply"; record=count,count/60,vector);
    @signal[rendezvousSuccess](type="int");
        @statistic[rendezvousSuccess](title="Rendezvous success"; source="rendezvousSuccess"; record=count,count/60,vector,mean);
    @signal[rendezvousFail](type="int");
        @statistic[rendezvousFail](title="Rendezvous fail"; source="rendezvousFail"; record=count,count/60,vector,mean);
    @signal[broadcastTimerCount](type="int");
        @statistic[broadcastTimerCount](title="broadcast Timer Count"; source="broadcastTimerCount"; record=count,count/60,vector,mean);
    @signal[txMobilitySignal](type="int");
        @statistic[txMobilitySignal](title="TX Mobility"; source="txMobilitySignal"; record=count,count/60,vector,mean);
    @signal[rxMobilitySignal](type="int");
        @statistic[rxMobilitySignal](title="RX Mobility"; source="rxMobilitySignal"; record=count,count/60,vector,mean);
        
    //@signal[stateMachineVector](type="int");
    	@statistic[stateMachineVector](title="StateMachine states"; source="stateMachineVector";enum="INIT=0,ONE=1,TWO=2,THREE=3,FOUR=4,FIVE=5,SIX=6";record=count,count/60,vector,histogram,mean);

    //@signal[eventSignal](type="int");
    	@statistic[eventSignal](title="Event incoming";source="eventSignal";enum="evSTEP=0,evFREE_CHANNEL_FOUND=1,evMOBILITY_SUCCESSFUL=2,evMOBILITY_TIMEOUT=3,evRENDEZVOUS_SUCCESS=4,evBROADCAST_TIMEOUT=5,evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED=6,evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED=7,evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED=8,evNO_COMMON_BACKUP_CHANNEL_AVAILABLE=9,evCHANNEL_SENSE_REPLY=10";record=count,count/60,vector,histogram,mean);
}
