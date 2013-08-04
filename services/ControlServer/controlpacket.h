#ifndef CONTROLPACKET_H
#define CONTROLPACKET_H

#include <commpacket.h>
#include <commpacketprocessor.h>

#include <QVariant>

class ControlPacket : public CommPacket
{
    enum Operation {
        RunCommand,

        StdInData,
        StdOutData,
        StdErrData
    };

public:
    inline ControlPacket(Operation o, QVariant d) : _op(o), _data(d) {}

    inline Operation operation() const{return _op;}

    template <class T>
    inline T data() const{return _data.value<T>();}

private:
    Operation _op;
    QVariant _data;
};

typedef CommPacketProcessor<ControlPacket> ControlPacketProcessor;

#endif // CONTROLPACKET_H
