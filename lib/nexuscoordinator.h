#ifndef NEXUSCOORDINATOR_H
#define NEXUSCOORDINATOR_H

#include "nexuscoordinator_global.h"

#include <QVariant>

class NEXUSCOORDINATORSHARED_EXPORT NexusCoordinator
{
    
public:
    NexusCoordinator();
    bool loadConfig(QVariantMap);

private:
    QVariantMap services;
};

#endif // NEXUSCOORDINATOR_H
