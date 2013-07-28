#ifndef COORDINATORLIBRARY_H
#define COORDINATORLIBRARY_H

#include <QLibrary>
#include <QVariantMap>
#include <QFileInfo>
#include <QDebug>
#include <QDir>

#include "coordinatorlibrarydef.h"
#include <nexusconfig.h>

#ifdef Q_OS_UNIX
#define LIB_EXT "so"
#else
#define LIB_EXT "dll"
#endif

class CoordinatorLibrary
{

    typedef QObject* (*CreateInstance)();
    typedef QByteArray (*LibraryMetaData)();
    typedef const char* (*LibraryAuthors)();
    typedef const char* (*LibraryVersion)();

    friend class NexusCoordinator;
public:
    inline QString version() const{
        return _version;
    }

    template<class T>
    inline T* newInstance() {
        if(!_createResolved) {
            _createInstance = (CreateInstance)_library->resolve(QString("%1CreateInstance").arg(_id).toLocal8Bit().data());
            _createResolved = true;
        }

        QObject* obj = _createInstance ? _createInstance() : 0;
        if(obj) {
            T* dObj = qobject_cast<T*>(obj);
            if(dObj)
                return dObj;
            obj->deleteLater();
            throw "Created object was not of correct type.";
        }

        throw "Failed to create instance.";
    }

    inline QVariantMap metaData() const{
        return _metaData;
    }

    inline QVariantMap defaultConfig() const{
        return _defaults;
    }

private:
    inline CoordinatorLibrary(QSharedPointer<QLibrary> lib = QSharedPointer<QLibrary>()) : _library(lib) {_createInstance = 0;_createResolved = 0;}

    static CoordinatorLibrary* create(QString name, QString type, QString path, bool exportSymbols =false);


    QString _id;
    QString _name;
    QString _version;
    QStringList _authors;
    bool _createResolved;
    QVariantMap _metaData;
    QVariantMap _defaults;
    CreateInstance _createInstance;
    QSharedPointer<QLibrary> _library;
    QList<CoordinatorLibraryRef> _modules;
};

#endif // COORDINATORLIBRARY_H
