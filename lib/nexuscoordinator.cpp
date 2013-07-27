#include "nexuscoordinator.h"
#include "coordinatorservice.h"
#include "coordinatorlibrary.h"

#include <QDebug>

NexusCoordinator* NexusCoordinator::_instance = 0;

bool NexusCoordinator::loadConfig(QVariantMap data) {
    QStringList takenNames;
    qDebug() << "Scanning services...";
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

            qDebug() << QString("%1 (%2)").arg(clazz).arg(name).toLocal8Bit().data();

            CoordinatorService* service = _services.value(name);
            if(service && service->_provider->_name != name) {
                service->deleteLater();
                service = 0;
            }

            if(!service) {
                CoordinatorServiceLibRef serviceProvider = _serviceLibraries.value(clazz);
                if(serviceProvider.isNull()) {
                    qDebug() << "Initializing service binary" << clazz;
                    serviceProvider = CoordinatorServiceLibRef(CoordinatorServiceLib::create(clazz, "Service", "services"));
                    if(serviceProvider)
                        _serviceLibraries.insert(clazz, serviceProvider);
                }

                if(serviceProvider) {
                    service = serviceProvider->newInstance();
                    if(service) {
                        _services.insert(name, service);
                        takenNames << name;

                        qDebug() << "Version" << serviceProvider->version();
                    } else
                        throw "newInstance failed";
                } else
                    throw "Failed to create instance of service library";
            }
        } catch(const char* reason) {
            qWarning() << "Failed to load:" << reason;
        }
    }

    return true;
}
