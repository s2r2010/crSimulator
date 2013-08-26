enum MobilityType
{
    MOB_DATA = 0;
    MOB_ACK = 1;
    MOB_NACK = 2;
    MOB_EVENT = 3;
    MOB_PUBLISH = 4;
    MOB_UPDATE = 5;
    MOB_KEEPALIVE = 6;
    MOB_UNKNOWN = 7;
}

packet MobilityPacket extends cPacket
{
    int kind @enum(MobilityType);
    int source;
    int destination;
    uint64_t seqno;
    int channelMap[];
}
