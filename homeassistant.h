#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QThread>
#include <QLoggingCategory>

#include "../remote-software/sources/integrations/integration.h"
#include "../remote-software/sources/integrations/plugininterface.h"
#include "../remote-software/sources/entities/entitiesinterface.h"
#include "../remote-software/sources/entities/entityinterface.h"
#include "../remote-software/sources/notificationsinterface.h"
#include "../remote-software/sources/yioapiinterface.h"
#include "../remote-software/sources/configinterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// HOME ASSISTANT FACTORY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HomeAssistant : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "YIO.PluginInterface" FILE "homeassistant.json")
    Q_INTERFACES(PluginInterface)

public:
    explicit HomeAssistant() :
        m_log("homeassistant")
    {}

    void create                     (const QVariantMap& config, QObject *entities, QObject *notifications, QObject* api, QObject *configObj) override;
    void setLogEnabled              (QtMsgType msgType, bool enable) override
    {
        m_log.setEnabled(msgType, enable);
    }
private:
    QLoggingCategory    m_log;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// HOME ASSISTANT BASE CLASS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HomeAssistantBase : public Integration
{
    Q_OBJECT

public:
    explicit HomeAssistantBase(QLoggingCategory& log, QObject *parent);
    virtual ~HomeAssistantBase();

    Q_INVOKABLE void setup  	    (const QVariantMap& config, QObject *entities, QObject *notifications, QObject* api, QObject *configObj);
    Q_INVOKABLE void connect	    ();
    Q_INVOKABLE void disconnect	    ();
    Q_INVOKABLE void sendCommand    (const QString& type, const QString& entity_id, const QString& command, const QVariant& param);

signals:
    void connectSignal              ();
    void disconnectSignal           ();
    void sendCommandSignal          (const QString& type, const QString& entity_id, const QString& command, const QVariant& param);


public slots:
    void stateHandler               (int state);

private:
    //void updateEntity               (const QString& entity_id, const QVariantMap& attr) {}

    QThread                         m_thread;
    QLoggingCategory&               m_log;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// HOME ASSISTANT THREAD CLASS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HomeAssistantThread : public QObject
{
    Q_OBJECT

public:
    HomeAssistantThread             (const QVariantMap &config, QObject *entities, QObject *notifications, QObject *api, QObject *configObj,
                                     QLoggingCategory& log);

signals:
    void stateChanged               (int state);

public slots:
    void connect                    ();
    void disconnect                 ();

    void sendCommand                (const QString& type, const QString& entity_id, const QString& command, const QVariant& param);

    void onTextMessageReceived	    (const QString &message);
    void onStateChanged             (QAbstractSocket::SocketState state);
    void onError                    (QAbstractSocket::SocketError error);

    void onTimeout                  ();


private:
    void webSocketSendCommand	    (const QString& domain, const QString& service, const QString& entity_id, QVariantMap *data);
    int  convertBrightnessToPercentage (float value);

    void updateEntity               (const QString& entity_id, const QVariantMap& attr);
    void updateLight                (EntityInterface* entity, const QVariantMap& attr);
    void updateBlind                (EntityInterface* entity, const QVariantMap& attr);
    void updateMediaPlayer          (EntityInterface* entity, const QVariantMap& attr);

    void setState                   (int state);

    EntitiesInterface*              m_entities;
    NotificationsInterface*         m_notifications;
    YioAPIInterface*                m_api;
    ConfigInterface*                m_config;

    QString                         m_id;

    QString                         m_ip;
    QString                         m_token;
    QWebSocket*                     m_socket;
    QTimer*                         m_websocketReconnect;
    int                             m_tries;
    int                             m_webSocketId;
    bool                            m_userDisconnect = false;
    QLoggingCategory&               m_log;
    int                             m_state = 0;

};


#endif // HOMEASSISTANT_H
