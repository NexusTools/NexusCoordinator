#ifndef TEXTCOMMANDPROCESSOR_H
#define TEXTCOMMANDPROCESSOR_H

#include <commpacketprocessor.h>

#include "controlpacket.h"

class TextCommandProcessor : public ControlPacketProcessor
{
public:
    explicit TextCommandProcessor();

protected:
    void processData(QByteArray&);
    
private:
};

#endif // TEXTCOMMANDPROCESSOR_H
