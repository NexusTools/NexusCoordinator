#include "nexuscoordinator.h"

#include <QDebug>

NexusCoordinator::NexusCoordinator()
{
}

bool NexusCoordinator::loadConfig(QVariantMap data) {
    qDebug() << "Loading services...";
    foreach(QVariant raw, data.value("Services").toList()) {
        try {
            QVariantMap service = raw.toMap();
            QString name = service.value("name").toString();
            QString clazz = service.value("class").toString();

            if(name.isEmpty())
                name = clazz;

            qDebug() << QString("%1 (%2)").arg(clazz).arg(name).toLocal8Bit().data();
        } catch(const char* reason) {
            qWarning() << "Failed to load" << reason;
        }
    }



    return true;
}
