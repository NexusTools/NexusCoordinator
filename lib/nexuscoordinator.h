#ifndef NEXUSCOORDINATOR_H
#define NEXUSCOORDINATOR_H

#include <QSharedPointer>
#include <QVariantMap>
#include <QStringList>

class CoordinatorService;

#include "coordinatorlibrary.h"

class NexusCoordinator : QObject
{
    Q_OBJECT

    friend class CoordinatorLibrary;
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

protected:
    CoordinatorService* createService(QString name, QString clazz, QVariantMap config =QVariantMap());
    CoordinatorLibraryRef loadModule(QString name);
    CoordinatorLibraryRef loadModule(QVariant def);

private:
    inline NexusCoordinator() {}

    static NexusCoordinator* _instance;
    QHash<QString, CoordinatorService*> _services;
    QHash<QString, CoordinatorLibraryPointer> _serviceLibraries;
    QHash<QString, CoordinatorLibraryPointer> _moduleLibraries;
    QList<CoordinatorLibraryRef> _configModules;
};

#endif // NEXUSCOORDINATOR_H
