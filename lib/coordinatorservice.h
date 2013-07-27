#ifndef COORDINATORSERVICE_H
#define COORDINATORSERVICE_H

#include <QVariantMap>
#include <QDebug>

#include "coordinatorlibrarydef.h"

class CoordinatorService : public QObject
{
    Q_OBJECT

    friend class NexusCoordinator;
public slots:
    inline bool start() {
        if(_running)
            return true;

        qDebug() << "Starting" << name();
        try {
            startImpl();
            _running = true;
        } catch(const char* err) {
            qWarning() << err;
            _error = err;
        } catch(QString err) {
            qWarning() << err.toLocal8Bit().data();
            _error = err;
        } catch(...) {}

        return _running;
    }
    inline bool stop() {
        if(!_running)
            return true;

        qDebug() << "Stopping" << name();
        try {
            stopImpl();
            _running = false;
        } catch(const char* err) {
            qWarning() << err;
            _error = err;
        } catch(QString err) {
            qWarning() << err.toLocal8Bit().data();
            _error = err;
        } catch(...) {}

        return !_running;
    }
    inline void restart() {
        stop();
        start();
    }
    inline QVariant config(QString name) {
        return config<QVariant>(name);
    }

    inline QString name() const{return _name;}

protected:
    inline CoordinatorService() {_running=false;}

    template<typename T>
    inline T config(QString name, T def =T()) {
        QVariant val = _config.value(name);
        if(val.canConvert<T>())
            return val.value<T>();
        if(!_provider.isNull()) {
            val = defaultConfig(name);
            if(val.canConvert<T>())
                return val.value<T>();
        }

        return def;
    }

    inline virtual void reloadConfigImpl() {}
    virtual void startImpl() =0;
    virtual void stopImpl() =0;

private:
    bool _running;
    QString _name;
    QString _error;
    QVariantMap _config;

    QVariant defaultConfig(QString);
    inline void setConfig(QVariantMap config) {
        _config = config;
        reloadConfigImpl();
    }

    CoordinatorLibraryRef _provider;
};

#endif // COORDINATORSERVICE_H
