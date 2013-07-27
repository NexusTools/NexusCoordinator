#ifndef COORDINATORLIBRARY_H
#define COORDINATORLIBRARY_H

#include <QLibrary>
#include <QVariantMap>
#include <QSharedPointer>

#include <nexusconfig.h>

#ifdef Q_OS_UNIX
#define LIB_EXT "so"
#else
#define LIB_EXT "dll"
#endif

template <class T>
class CoordinatorLibrary
{

    typedef T* (*CreateInstance)();
    typedef QByteArray (*LibraryMetaData)();
    typedef const char* (*LibraryVersion)();

    friend class NexusCoordinator;
public:
    inline QString version() const{
        return _version;
    }

    inline T* newInstance() {
        if(!_createResolved) {
            _createInstance = (CreateInstance)_library->resolve(QString("%1CreateInstance").arg(_id).toLocal8Bit().data());
            _createResolved = true;
        }

        return _createInstance ? _createInstance() : 0;
    }

    inline QVariantMap metaData() const{
        return _metaData;
    }

    inline QVariantMap defaultConfig() const{
        return _defaults;
    }

private:
    inline CoordinatorLibrary(QSharedPointer<QLibrary> lib = QSharedPointer<QLibrary>()) : _library(lib) {_createInstance = 0;_createResolved = 0;}

    static inline CoordinatorLibrary* create(QString name, QString type, QString path, bool exportSymbols =false) {
        QSharedPointer<QLibrary> lib(new QLibrary(
#ifdef IDE_MODE
                "../" +
#endif
                path + '/' +
#ifdef IDE_MODE
                name + '/' +
#endif
                name + '.' + LIB_EXT));

        if(lib->load()) {
            QString ID = QString("NexusCoordinator_%1_%2_").arg(type).arg(name);
            CreateInstance createInstance = (CreateInstance)lib->resolve(QString("%1CreateInstance").arg(ID).toLocal8Bit().data());
            if(!createInstance)
                throw "Missing CreateInstance entrypoint";

            LibraryVersion libVer = (LibraryVersion)lib->resolve(QString("%1Version").arg(ID).toLocal8Bit().data());
            LibraryMetaData libMeta = (LibraryMetaData)lib->resolve(QString("%1MetaData").arg(ID).toLocal8Bit().data());

            QString version;
            if(libVer)
                version = libVer();

            QVariantMap metaData;
            if(libMeta) {
                QByteArray metaBytes = libMeta();
                if(!metaBytes.isEmpty()) {
                    metaData = NexusConfig::parse(metaBytes, NexusConfig::XmlFormat).toMap();
                    qDebug() << "MetaData" << metaData;
                }
            }

            if(exportSymbols) {
                lib->unload();
                lib->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
                if(!lib->load())
                    throw lib->errorString().toLocal8Bit().data();
            }

            CoordinatorLibrary* library = new CoordinatorLibrary(lib);

            library->_id = ID;
            library->_name = name;
            library->_version = version;
            library->_metaData = metaData;
            library->_defaults = library->_metaData.take("DefaultConfig").toMap();

            return library;
        } else
            throw lib->errorString().toLocal8Bit().data();
    }


    QString _id;
    QString _name;
    QString _version;
    bool _createResolved;
    QVariantMap _metaData;
    QVariantMap _defaults;
    CreateInstance _createInstance;
    QSharedPointer<QLibrary> _library;
};

class CoordinatorService;
class CoordinatorModule;

typedef CoordinatorLibrary<CoordinatorService> CoordinatorServiceLib;
typedef QSharedPointer<CoordinatorServiceLib> CoordinatorServiceLibRef;

typedef CoordinatorLibrary<CoordinatorModule> CoordinatorModuleLib;
typedef QSharedPointer<CoordinatorModuleLib> CoordinatorModuleLibRef;

#endif // COORDINATORLIBRARY_H
