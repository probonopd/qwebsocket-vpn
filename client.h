#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include <QtWebSockets/QWebSocket>
#include "qaesencryption.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QString &url, QObject *parent = nullptr, QByteArray _key = QByteArray());

private:
    QWebSocket m_webSocket;
    QAESEncryption aes;
    QByteArray key;
    QTimer pingTimer;
    bool inited; //true if first msg which contains client ip is received
};

#endif // CLIENT_H
