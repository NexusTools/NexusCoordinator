#ifndef COORDINATORLIBRARYDEF_H
#define COORDINATORLIBRARYDEF_H

#include <QSharedPointer>

class CoordinatorLibrary;

typedef QSharedPointer<CoordinatorLibrary> CoordinatorLibraryRef;
typedef QWeakPointer<CoordinatorLibrary> CoordinatorLibraryPointer;

#endif // COORDINATORLIBRARYDEF_H
