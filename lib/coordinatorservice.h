#ifndef COORDINATORSERVICE_H
#define COORDINATORSERVICE_H

#include <QVariantMap>

#include "coordinatorlibrary.h"

class CoordinatorService : public QObject
{
    Q_OBJECT

    friend class NexusCoordinator;
public slots:
    inline bool start() {
        if(_running)
            return true;

        try {
            startImpl();
            _running = true;
        } catch(const char* err) {
            _error = err;
        } catch(QString err) {
            _error = err;
        } catch(...) {}

        return _running;
    }
    inline bool stop() {
        if(!_running)
            return true;

        try {
            stopImpl();
            _running = false;
        } catch(const char* err) {
            _error = err;
        } catch(QString err) {
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

protected:
    template<typename T>
    inline T config(QString name, T def =T()) {
        QVariant val = _config.value(name);
        if(val.canConvert<T>())
            return val.value<T>();
        if(!_provider.isNull()) {
            val = _provider->defaultConfig().value(name);
            if(val.canConvert<T>())
                return val.value<T>();
        }

        return def;
    }

protected:
    inline CoordinatorService() {_running=false;}

    virtual void reloadConfigImpl() =0;
    virtual void startImpl() =0;
    virtual void stopImpl() =0;

private:
    bool _running;
    QString _name;
    QString _error;
    QVariantMap _config;

    inline void setConfig(QVariantMap config) {
        _config = config;
        reloadConfigImpl();
    }

    CoordinatorServiceLibRef _provider;
};

#endif // COORDINATORSERVICE_H
