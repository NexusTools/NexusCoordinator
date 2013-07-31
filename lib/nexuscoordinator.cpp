#include "nexuscoordinator.h"
#include "coordinatorservice.h"

#include <QDebug>

NexusCoordinator* NexusCoordinator::_instance = 0;

NexusCoordinator::NexusCoordinator() {
    registerType("Service", "services", "Modules");
    registerType("Module", "modules", "Modules");
}

bool NexusCoordinator::readConfig(QVariantMap data) {
    _configModules.clear();
    qDebug() << "Checking dependancies ...";
    foreach(QVariant raw, data.value("Modules").toList()) {
        Module::Ref lib = loadModule(raw);

        if(lib)
            _configModules.append(lib);
    }

    QStringList takenNames;
    qDebug() << "Configuring services ...";
    foreach(QVariant raw, data.value("Services").toList()) {
        try {
            QVariantMap serviceConfig = raw.toMap();
            QString name = serviceConfig.value("name").toString().trimmed();
            QString clazz = serviceConfig.value("class").toString().trimmed();

            if(clazz.isEmpty())
                throw "Config error, service missing class";

            if(name.isEmpty()) {
                name = clazz;

                int id = 2;
                while(takenNames.contains(name))
                    name = QString("%1-%2").arg(clazz).arg(id++);
            } else if(takenNames.contains(name))
                throw QString("%1 is already registered.").arg(name).toLocal8Bit().data();

            createService(name, clazz, serviceConfig);
            takenNames << name;
		} catch(const char* reason) {
			qWarning() << "Failed to load:" << reason;
		} catch(QString reason) {
			qWarning() << "Failed to load:" << reason.toLocal8Bit().data();
		} catch(...) {
            qCritical() << "Unknown exception caught while loading!";
		}
    }

    return true;
}

CoordinatorService* NexusCoordinator::createService(QString name, QString clazz, QVariantMap config) {
    CoordinatorService* service = _services.value(name);
    if(service && (!service->provider() ||
                   service->provider()->name() != clazz)) {
        service->deleteLater();
        service = 0;
    }

    if(!service) {
        Module::Ref serviceProvider = ModularCore::loadModule(clazz, "Service");

        if(serviceProvider) {
            service = serviceProvider->createCompatiblePlugin<CoordinatorService>();
            if(service) {
                qDebug() << service;
                service->_name = name;
                service->setConfig(config);
                _services.insert(name, service);
            } else
                throw "createCompatiblePlugin<CoordinatorService>() failed";
        } else
            throw "Failed to create instance of service library";
    }

    return service;
}

Module::Ref NexusCoordinator::loadModule(QVariant def) {
    QString clazz = def.toMap().value("class").toString();
    Module::Ref mod;
    if(!clazz.isEmpty())
        try {
            mod = ModularCore::loadModule(clazz, "Module");
        } catch(const char* reason) {
            qWarning() << "Failed to load:" << reason;
        } catch(QString reason) {
            qWarning() << "Failed to load:" << reason.toLocal8Bit().data();
        } catch(...) {
            qWarning() << "Unknown exception caught while loading...";
        }

    return mod;
}

QVariant CoordinatorService::defaultConfig(QString key) {
    return provider()->metaData().value("DefaultConfig").toMap().value(key);
}
