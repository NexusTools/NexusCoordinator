#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include <commclient.h>

#include "controlpacket.h"

class ControlClient : public CommClient<ControlPacket, ControlPacketProcessor>
{
private:
    void processPacket(P&) =0;
    void sendData(QByteArray&) =0;
}

#endif // CONTROLCLIENT_H
