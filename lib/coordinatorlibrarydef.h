#ifndef COORDINATORLIBRARYDEF_H
#define COORDINATORLIBRARYDEF_H

#include <QSharedPointer>

template <class T> class CoordinatorLibrary;
class CoordinatorService;
class CoordinatorModule;

typedef CoordinatorLibrary<CoordinatorService> CoordinatorServiceLib;
typedef QSharedPointer<CoordinatorServiceLib> CoordinatorServiceLibRef;

typedef CoordinatorLibrary<CoordinatorModule> CoordinatorModuleLib;
typedef QSharedPointer<CoordinatorModuleLib> CoordinatorModuleLibRef;

#endif // COORDINATORLIBRARYDEF_H
