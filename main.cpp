#include <QCoreApplication>
#include <QTime>
#include <QCommandLineParser>
#include <QDebug>
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qsrand(QTime::currentTime().msec());

    QCommandLineParser parser;
    parser.setApplicationDescription("QtWebSockets VPN");
    parser.addHelpOption();

    QCommandLineOption modeOption(QStringList() << "m" << "mode",
            "server or client mode",
            "mode", "");
    parser.addOption(modeOption);

    QCommandLineOption urlOption(QStringList() << "u" << "url",
            "websocket server url, ws://1.2.3.4:18888. Client only",
            "url", "");
    parser.addOption(urlOption);

    QCommandLineOption portOption(QStringList() << "p" << "port",
            QCoreApplication::translate("main", "Port to listen [default: 18888]."),
            QCoreApplication::translate("main", "port"), QLatin1Literal("18888"));
    parser.addOption(portOption);

    QCommandLineOption passwdOption(QStringList() << "s" << "pass",
            QCoreApplication::translate("main", "AES password"),
            QCoreApplication::translate("main", "password"), QLatin1Literal("zxcvbnm"));
    parser.addOption(passwdOption);

    parser.process(a);

    int port = parser.value(portOption).toInt();
    QString mode = parser.value(modeOption);
    QString url = parser.value(urlOption);
    QByteArray key = parser.value(passwdOption).toLatin1();
    if(mode == "server"){
        new Server(nullptr, port, key);
        qWarning()<<"Running in server mode, listening local port "<< port;

    }else if(mode == "client"){
        new Client(url, nullptr, key);
        qWarning()<<"Running in client mode, server is "<< url;
    }else{
        qDebug()<<"invalid mode "<<mode<<", should be client or server";
        return -1;
    }
    return a.exec();
}
