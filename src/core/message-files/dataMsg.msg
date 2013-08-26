
enum dataType
{
    RTS = 100;
    CTS = 101;
    ACK = 102;
    RTSNACK = 103;
    DATA = 104;
    DATAACK = 105;
    DATANACK = 106;
    EOT = 107;
    PUSTART = 108;
    PUEND = 109;
    DRMCH = 110;   // A recommendation message from DRM
    MACDATA = 111;
    MACACK = 112;
    MOBILITY = 113;
}

packet dataMsg extends cPacket
{
    //string displayString = "i=old/ball,red,is=s";
    int kind @enum(dataType);
    int numberOfPackets;
    int proposedChannel;
    int source;
    int destination;
}
