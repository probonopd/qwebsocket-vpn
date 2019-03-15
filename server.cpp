#include "server.h"
#include "common.h"
#include <QSocketNotifier>
#include <QDebug>

Server::Server(QObject *parent, quint16 port) : QObject(parent)
{
    w_server = new QWebSocketServer("WEBSOCKET VPN", QWebSocketServer::NonSecureMode, this);
    w_server->listen(QHostAddress::Any, port);
    connect(w_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

}

void Server::onNewConnection()
{
    QWebSocket *pSocket = w_server->nextPendingConnection();
    qint32 sock = create_tun_interface();
    if(sock<0){
        qWarning()<<"Can not create tun interface";
        return;
    }
    QSocketNotifier *sn_r = new QSocketNotifier(sock, QSocketNotifier::Read, nullptr);
    QSocketNotifier *sn_e = new QSocketNotifier(sock, QSocketNotifier::Exception, nullptr);

    connect(pSocket, &QWebSocket::binaryMessageReceived, [=](const QByteArray &msg){
        ::write(sock, msg.constData(), msg.length());
    });
    connect(pSocket, &QWebSocket::disconnected, [=]{
        ::close(sock);
        pSocket->deleteLater();
    });
    connect(sn_r, &QSocketNotifier::activated, [pSocket](int fd){
        char buf[4096];
        int nbytes = ::read(fd, buf, sizeof(buf));
        if(nbytes<=0){
            return;
        }
        pSocket->sendBinaryMessage(QByteArray(buf, nbytes));
    });

    connect(sn_e, &QSocketNotifier::activated, [sn_e]{
        qWarning()<<"delete QSocketNotifier";
        sn_e->deleteLater();
    });
}

