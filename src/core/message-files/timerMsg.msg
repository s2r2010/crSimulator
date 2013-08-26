enum timerType
{
    publishSENSING = 100;
    senseFREE = 101;
    senseDATA = 102;
    sensePROPOSED = 103;    
}

message timerMsg
{
    int type;
    int newType @enum(timerType); 
}
