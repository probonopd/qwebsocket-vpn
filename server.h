#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMap>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "qaesencryption.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr, quint16 port = 0, QByteArray _key = QByteArray());

private:
    QWebSocketServer *w_server;
    QAESEncryption aes;
    QByteArray key;
    QMap<quint8, QWebSocket*> clientMap;
};

#endif // SERVER_H
