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

template <class T>
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

    static inline CoordinatorLibrary* create(QString name, QString type, QString path, bool exportSymbols =false) {
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
            library->_version = version;
            library->_metaData = metaData;
            library->_defaults = library->_metaData.take("DefaultConfig").toMap();

            return library;
        } else
			throw lib->errorString();
    }


    QString _id;
    QString _name;
    QString _version;
    QStringList _authors;
    bool _createResolved;
    QVariantMap _metaData;
    QVariantMap _defaults;
    CreateInstance _createInstance;
    QSharedPointer<QLibrary> _library;
};

#endif // COORDINATORLIBRARY_H
