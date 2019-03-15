#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr, quint16 port = 0);

private slots:
    void onNewConnection();
private:
    QWebSocketServer *w_server;

};

#endif // SERVER_H
