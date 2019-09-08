#include "client.h"
#include "common.h"
#include <QDebug>
#include <QSocketNotifier>
#include <QCoreApplication>
#include <QHostAddress>
#include <QProcess>
#include <arpa/inet.h>

Client::Client(const QString &url, QObject *parent, QByteArray _key) : QObject(parent), key(_key), inited(false), aes(QAESEncryption::AES_256, QAESEncryption::ECB)
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
    //QProcess::execute(QString("ifconfig ")+VPN_IF+" 10.200.200.2 netmask 255.255.255.252 up");
    QSocketNotifier *sn_r = new QSocketNotifier(sock, QSocketNotifier::Read, nullptr);
    QSocketNotifier *sn_e = new QSocketNotifier(sock, QSocketNotifier::Exception, nullptr);

    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, [=](const QByteArray &_msg){
        QByteArray msg = aes.removePadding(aes.decode(_msg, key));
        if(!inited){
            inited = true;
            Q_ASSERT(msg.length() == sizeof(quint32));
            quint32 clientIp = *(const quint32*)msg.constData();
            QProcess::execute(QString("ifconfig ")+VPN_IF+" " + QHostAddress(clientIp).toString() + " netmask 255.255.255.0 up");
        }else{
            ::write(sock, msg.constData(), msg.length());
        }
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
        m_webSocket.sendBinaryMessage(aes.encode(QByteArray(buf, nbytes), key));
    });

    connect(sn_e, &QSocketNotifier::activated, [sn_e]{
        qWarning()<<"delete QSocketNotifier";
        sn_e->deleteLater();
    });
}


