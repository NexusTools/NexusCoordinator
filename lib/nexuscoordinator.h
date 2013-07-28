#ifndef NEXUSCOORDINATOR_H
#define NEXUSCOORDINATOR_H

#include <coordinator-macros.h>
#include <modularcore.h>

#include <QSharedPointer>
#include <QVariantMap>
#include <QStringList>

class CoordinatorService;

class NexusCoordinator : public QObject, public ModularCore
{
    Q_OBJECT
    MODULAR_CORE

    typedef QHash<QString, CoordinatorService*> RegisteredServices;

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
    inline void registerService(QString name, CoordinatorService*);

protected:
    CoordinatorService* createService(QString name, QString clazz, QVariantMap config =QVariantMap());
    Module::Ref loadModule(QVariant def);

    inline void moduleVerify(Module::Ref module) {
        module->load(module->type() == "Module" ? Module::LoadFlags(Module::LooseVerify|Module::ExportSymbols|Module::IgnoreLibraryName) : Module::LoadFlags(Module::StrictVerify));
    }

private:
    NexusCoordinator();

    static NexusCoordinator* _instance;

    Module::List _configModules;
    RegisteredServices _services;
};

#endif // NEXUSCOORDINATOR_H
