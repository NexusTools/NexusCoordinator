#ifndef TEXTCOMMANDPROCESSOR_H
#define TEXTCOMMANDPROCESSOR_H

#include <commpacketprocessor.h>
#include <QObject>

class TextCommandProcessor : public QObject
{
    Q_OBJECT
public:
    explicit TextCommandProcessor();

protected:
    void processData(QByteArray&);
    
private:
};

#endif // TEXTCOMMANDPROCESSOR_H
