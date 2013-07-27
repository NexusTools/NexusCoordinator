#include "nexuscoordinator.h"
#include "coordinatorservice.h"
#include "coordinatorlibrary.h"

#include <QDebug>

NexusCoordinator* NexusCoordinator::_instance = 0;

bool NexusCoordinator::loadConfig(QVariantMap data) {


    _configModules.clear();
    qDebug() << "Loading modules...";
    foreach(QVariant raw, data.value("Modules").toList()) {
        CoordinatorLibraryRef lib = loadModule(raw);

        if(lib)
            _configModules.append(lib);
    }

    QStringList takenNames;
    qDebug() << "Loading services...";
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
			qWarning() << "Unknown exception caught while loading...";
		}
    }

    return true;
}

CoordinatorService* NexusCoordinator::createService(QString name, QString clazz, QVariantMap config) {
    qDebug() << QString("%1 (%2)").arg(clazz).arg(name).toLocal8Bit().data();

    CoordinatorService* service = _services.value(name);
    if(service && service->_provider->_name != name) {
        service->deleteLater();
        service = 0;
    }

    if(!service) {
        CoordinatorLibraryRef serviceProvider = _serviceLibraries.value(clazz).toStrongRef();
        if(serviceProvider.isNull()) {
            qDebug() << "Initializing service binary" << clazz;
            serviceProvider = CoordinatorLibraryRef(CoordinatorLibrary::create(clazz, "Service", "services"));
            if(serviceProvider)
                _serviceLibraries.insert(clazz, serviceProvider.toWeakRef());
        }

        if(serviceProvider) {
            service = serviceProvider->newInstance<CoordinatorService>();
            if(service) {
                service->_name = name;
                service->_provider = serviceProvider;
                service->setConfig(config);

                _services.insert(name, service);
            } else
                throw "newInstance failed";
        } else
            throw "Failed to create instance of service library";
    }

    return service;
}

CoordinatorLibraryRef NexusCoordinator::loadModule(QVariant def) {
    QString clazz = def.toMap().value("class").toString();
    CoordinatorLibraryRef mod;
    if(!clazz.isEmpty())
        mod = loadModule(clazz);
    return mod;
}

CoordinatorLibraryRef NexusCoordinator::loadModule(QString clazz) {
    CoordinatorLibraryRef module(_moduleLibraries.value(clazz).toStrongRef());
    if(module.isNull()) {
        qDebug() << "Loading module" << clazz;
        try {
            module = CoordinatorLibraryRef(CoordinatorLibrary::create(clazz, "Module", "modules"));
            if(module)
                _moduleLibraries.insert(clazz, module.toWeakRef());
        } catch(const char* reason) {
            qWarning() << "Failed to load:" << reason;
        } catch(QString reason) {
            qWarning() << "Failed to load:" << reason.toLocal8Bit().data();
        } catch(...) {
            qWarning() << "Unknown exception caught while loading...";
        }
    }

    return module;
}

QVariant CoordinatorService::defaultConfig(QString key) {
    return _provider->defaultConfig().value(key);
}

CoordinatorLibrary* CoordinatorLibrary::create(QString name, QString type, QString path, bool exportSymbols) {
#ifdef IDE_MODE
        static QString basePath = QFileInfo(QDir::currentPath()).dir().path();
#endif
        QString libPath =
#ifdef IDE_MODE
                basePath + '/' +
#endif
                path + '/' +
#ifdef IDE_MODE
                name + '/' +
#ifdef Q_OS_WIN
#ifdef DEBUG_MODE
                "debug/" +
#else
                "release/" +
#endif
#else
                "lib" +
#endif
#endif
                name +
#ifdef IDE_MODE
#ifdef Q_OS_WIN
                "0" +
#endif
#endif
                '.' + LIB_EXT;

    QVariantMap metaData;
    {
        qDebug() << libPath;
        QFile f(libPath);
        if(f.open(QFile::ReadOnly)) {
            int pos;
            QByteArray buffer;
            while(!f.atEnd()) {
                if(buffer.length() >= 200)
                    buffer = buffer.mid(100);
                buffer += f.read(100);
                if((pos = buffer.indexOf("<CoordinatorLibrary>")) > -1) {
                    bool ok;
                    quint16 size;
                    size = buffer.mid(pos-4, 4).toHex().toInt(&ok, 16);
                    if(!ok)
                        throw "MetaData size corrupt.";

                    buffer = buffer.mid(pos);
                    size -= buffer.length();
                    while(size > 0) {
                        if(f.atEnd())
                            throw "EOF while parsing metadata.";

                        QByteArray newData = f.read(size);
                        buffer += newData;
                        size -= newData.length();
                    }

                    metaData = NexusConfig::parse(buffer, NexusConfig::XmlFormat, "CoordinatorLibrary").toMap();
                    break;
                }
            }
        } else
            throw "Could not open binary...";
    }

    QList<CoordinatorLibraryRef> deps;
    foreach(QVariant module, metaData.value("Modules").toList()) {
        CoordinatorLibraryRef mod = NexusCoordinator::instance()->loadModule(module);
        if(!mod.isNull())
            deps.append(mod);
    }

    QSharedPointer<QLibrary> lib(new QLibrary(libPath));
    if(lib->load()) {
        QString ID = QString("NexusCoordinator_%1_%2_").arg(type).arg(name);

        LibraryVersion libVer = (LibraryVersion)lib->resolve(QString("%1Version").arg(ID).toLocal8Bit().data());
        LibraryAuthors libAuthors = (LibraryAuthors)lib->resolve(QString("%1Authors").arg(ID).toLocal8Bit().data());

        QString version;
        if(libVer)
            version = libVer();

        QStringList authors;
        if(libAuthors)
            authors = QString::fromUtf8(libAuthors()).split(',', QString::SkipEmptyParts);
        if(authors.isEmpty())
            authors << "Unknown";
        else
            qDebug() << "Authors" << authors;

        if(exportSymbols) {
            lib->unload();
            lib->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
            if(!lib->load())
                throw lib->errorString();
        }

        CoordinatorLibrary* library = new CoordinatorLibrary(lib);

        library->_id = ID;
        library->_name = name;
        library->_deps = deps;
        library->_version = version;
        library->_metaData = metaData;
        library->_defaults = library->_metaData.take("DefaultConfig").toMap();

        return library;
    } else
        throw lib->errorString();
}
