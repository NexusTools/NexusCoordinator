#ifndef NEXUSCOORDINATOR_H
#define NEXUSCOORDINATOR_H

#include <QSharedPointer>
#include <QVariantMap>
#include <QStringList>

#include "coordinatorlibrary.h"

class NexusCoordinator
{
public:
    static inline NexusCoordinator* instance() {return _instance;}

    static inline void init(QVariantMap config =QVariantMap()) {
        if(!_instance)
            _instance = new NexusCoordinator();
        if(!config.isEmpty())
            _instance->loadConfig(config);
    }

    bool loadConfig(QVariantMap);

    inline CoordinatorService* service(QString name) const{return _services.value(name);}
    inline QList<CoordinatorService*> services() const{return _services.values();}

private:
    inline NexusCoordinator() {}

    static NexusCoordinator* _instance;
    QHash<QString, CoordinatorService*> _services;
    QHash<QString, CoordinatorServiceLibRef> _serviceLibraries;
};

#endif // NEXUSCOORDINATOR_H
