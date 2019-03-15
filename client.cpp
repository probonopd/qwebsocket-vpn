#include "client.h"
#include "common.h"
#include <QDebug>
#include <QSocketNotifier>
#include <QCoreApplication>
#include <arpa/inet.h>

Client::Client(const QString &url, QObject *parent) : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, []{
        qWarning()<<"Connection successful!";
    });
    m_webSocket.open(QUrl(url));
    qint32 sock = create_tun_interface();
    if(sock<0){
        qWarning()<<"Can not create tun interface";
        QCoreApplication::exit(-1);
    }
    QSocketNotifier *sn_r = new QSocketNotifier(sock, QSocketNotifier::Read, nullptr);
    QSocketNotifier *sn_e = new QSocketNotifier(sock, QSocketNotifier::Exception, nullptr);

    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, [=](const QByteArray &msg){
        ::write(sock, msg.constData(), msg.length());
    });
    connect(&m_webSocket, &QWebSocket::disconnected, [=]{
        ::close(sock);
    });
    connect(sn_r, &QSocketNotifier::activated, [this](int fd){
        char buf[4096];
        int nbytes = ::read(fd, buf, sizeof(buf));
        if(nbytes<=0){
            return;
        }
        m_webSocket.sendBinaryMessage(QByteArray(buf, nbytes));
    });

    connect(sn_e, &QSocketNotifier::activated, [sn_e]{
        qWarning()<<"delete QSocketNotifier";
        sn_e->deleteLater();
    });
}


