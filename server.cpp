#include "server.h"
#include "common.h"
#include <QSocketNotifier>
#include <QDebug>
#include <QProcess>

Server::Server(QObject *parent, quint16 port, QByteArray _key) : QObject(parent), key(_key), aes(QAESEncryption::AES_256, QAESEncryption::ECB)
{
    w_server = new QWebSocketServer("WEBSOCKET VPN", QWebSocketServer::NonSecureMode, this);
    w_server->listen(QHostAddress::Any, port);
    int tunSock = create_tun_interface();
    if(tunSock<0){
        qWarning()<<"Can not create tun interface";
        return;
    }
    QProcess::execute(QString("ifconfig ")+VPN_IF+" 10.200.200.1 netmask 255.255.255.0 up");
    QSocketNotifier *sn_r = new QSocketNotifier(tunSock, QSocketNotifier::Read, nullptr);
    connect(sn_r, &QSocketNotifier::activated, [this](int fd){
        char buf[4096];
        int nbytes = ::read(fd, buf, sizeof(buf));
        if(nbytes<=0){
            return;
        }
        //drop ipv6 packet
        if((buf[0]>>4)!= 4) return;
        //use dest ip to findout dest client
        quint32 offset = ntohl(*(quint32*)&buf[16]) - ntohl(inet_addr("10.200.200.0"));
        if(offset > 255) return;
        QWebSocket *pSocket = clientMap.value(offset);
        if(pSocket){
            pSocket->sendBinaryMessage(aes.encode(QByteArray(buf, nbytes), key));
        }
    });

    connect(w_server, &QWebSocketServer::newConnection, [this, tunSock]{
        QWebSocket *pSocket = w_server->nextPendingConnection();
        quint8 offset = [this](){
            //allocate ip address for client
            for(quint8 i = 2; i < 255; i++){
                if(!clientMap.contains(i)) return i;
            }
            return quint8(0);
        }();
        if(offset == 0){
            pSocket->close();
            pSocket->deleteLater();
            return;
        }
        clientMap[offset] = pSocket;
        connect(pSocket, &QWebSocket::binaryMessageReceived, [=](const QByteArray &_msg){
            QByteArray msg = aes.removePadding(aes.decode(_msg, key));
            //check if dest is another client and forward it directly
            const char *pkt = msg.constData();
            if((pkt[0]>>4)!= 4) return;
            quint32 offset = ntohl(*(quint32*)&pkt[16]) - ntohl(inet_addr("10.200.200.0"));
            if(offset < 255 && offset > 1){
                QWebSocket *pSocket = clientMap.value(offset);
                if(pSocket){
                    pSocket->sendBinaryMessage(_msg);
                }
            }else if(offset == 0 || offset == 255){ 
	        //broadcast pkt
                ::write(tunSock, msg.constData(), msg.length());
		for(auto pSocket : clientMap){
		    if(pSocket && pSocket != QObject::sender()){
		        pSocket->sendBinaryMessage(_msg);
		    }
		}
	    }else{
                ::write(tunSock, msg.constData(), msg.length());
            }

        });
        connect(pSocket, &QWebSocket::disconnected, [=]{
            pSocket->deleteLater();
            clientMap.remove(offset);
        });
        //send first msg to client, which contains client's ip addr.
        quint32 clientIp = ntohl(inet_addr("10.200.200.0")) + offset;
        pSocket->sendBinaryMessage(aes.encode(QByteArray((char*)&clientIp, sizeof(clientIp)), key));
    });

}

