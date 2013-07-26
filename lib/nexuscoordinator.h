#ifndef NEXUSCOORDINATOR_H
#define NEXUSCOORDINATOR_H

#include "nexuscoordinator_global.h"
#include "commservergroup.h"

class NEXUSCOORDINATORSHARED_EXPORT NexusCoordinator
{
    
public:
    NexusCoordinator();

private:
    CommServerGroup* commGroup;

};

#endif // NEXUSCOORDINATOR_H
