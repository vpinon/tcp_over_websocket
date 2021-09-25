#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QDebug>
#include "tcp2web.h"
void Tcp2WebSocket::webRead(const QByteArray &message) {
    m_tcpSocket->write(message);
}
void Tcp2WebSocket::tcpRead() {
    m_webSocket->sendBinaryMessage(m_tcpSocket->readAll());
}
void Tcp2WebSocket::tcpConnected() {
    qDebug() << "TCP Socket connected";
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &Tcp2WebSocket::webRead);
    connect(m_tcpSocket, &QIODevice::readyRead, this, &Tcp2WebSocket::tcpRead);
}
void Tcp2WebSocket::webDisconnected() {
    qWarning() << "WebSocket closed";
    if (m_tcpSocket->isValid()) m_tcpSocket->close();
    deleteLater();
}
void Tcp2WebSocket::tcpConnectionError(QAbstractSocket::SocketError error) {
    qWarning() << error << m_tcpSocket->errorString();
    m_webSocket->close();
}
Tcp2WebSocket::Tcp2WebSocket(QWebSocket *webSocket, const QUrl &tcpUrl, QObject *parent):
    QObject(parent), m_webSocket(webSocket) {
    m_tcpSocket = new QTcpSocket();
    connect(m_webSocket, &QWebSocket::disconnected, this, &Tcp2WebSocket::webDisconnected);
    connect(m_tcpSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Tcp2WebSocket::tcpConnectionError);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &Tcp2WebSocket::tcpConnected);
    m_tcpSocket->connectToHost(tcpUrl.host(), tcpUrl.port());
    qDebug() << "TCP connexion at" << tcpUrl;
}
void Tcp2WebServer::webConnection() {
    new Tcp2WebSocket(m_webServer.nextPendingConnection(), m_tcpUrl, this);
}
Tcp2WebServer::Tcp2WebServer(const QUrl &tcpUrl, quint16 webPort, QObject *parent):
    QObject(parent), m_tcpUrl(tcpUrl), m_webServer("TCP to WebSocket bridge", QWebSocketServer::NonSecureMode, this) {
    qDebug() << "Starting WebSocket server on port " << webPort;
    if (m_webServer.listen(QHostAddress::Any, webPort)) {
        connect(&m_webServer, &QWebSocketServer::newConnection, this, &Tcp2WebServer::webConnection);
    } else {
        qWarning() << "Failed to open server on port " << webPort;
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("TCP to WebSocket bridge");
    parser.addHelpOption();
    QCommandLineOption tcpUrlOption(QStringList() << "u" << "tcpUrl",
            QCoreApplication::translate("main", "TCP URL to connect to redirect [default: 127.0.0.1:22]."),
            QCoreApplication::translate("main", "tcpUrl"), "127.0.0.1:22");
    parser.addOption(tcpUrlOption);
    QCommandLineOption webPortOption(QStringList() << "w" << "webPort",
            QCoreApplication::translate("main", "WebSocket port to serve [default: 0xF055]."),
            QCoreApplication::translate("main", "tcpPort"), "0xF055");
    parser.addOption(webPortOption);
    parser.process(a);

    Tcp2WebServer server(parser.value(tcpUrlOption), parser.value(webPortOption).toInt());

    return a.exec();
}
