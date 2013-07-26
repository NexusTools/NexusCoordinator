#ifndef COORDINATORSERVICE_H
#define COORDINATORSERVICE_H

#include <QObject>

class CoordinatorService : public QObject
{
    Q_OBJECT
public:
    explicit CoordinatorService(QObject *parent = 0);
    
};

#endif // COORDINATORSERVICE_H
