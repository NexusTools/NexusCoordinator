#ifndef COORDINATORSERVICE_H
#define COORDINATORSERVICE_H

#include <QObject>

class CoordinatorService : public QObject
{
    Q_OBJECT
public:
    inline bool start() {

    }
    inline bool stop() {

    }
    inline void restart() {

    }

protected:
    inline CoordinatorService() {}

    virtual void startImpl() =0;
    virtual void stopImpl() =0;

private:
    void setConfig();
};

#endif // COORDINATORSERVICE_H
