#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QString &url, QObject *parent = nullptr);

private:
    QWebSocket m_webSocket;
};

#endif // CLIENT_H
