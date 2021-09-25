#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QDebug>
#include "web2tcp.h"
void Web2TcpSocket::tcpRead() {
    m_webSocket->sendBinaryMessage(m_tcpSocket->readAll());
}
void Web2TcpSocket::webRead(const QByteArray &message) {
    m_tcpSocket->write(message);
}
void Web2TcpSocket::webConnected() {
    qDebug() << "Web Socket connected";
    connect(m_tcpSocket, &QIODevice::readyRead, this, &Web2TcpSocket::tcpRead);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &Web2TcpSocket::webRead);
}
void Web2TcpSocket::tcpDisconnected() {
    qWarning() << "TCP Socket closed";
    if (m_webSocket->isValid()) m_webSocket->close();
    deleteLater();
}
void Web2TcpSocket::webConnectionError(QAbstractSocket::SocketError error) {
    qWarning() << error << m_webSocket->errorString();
    m_tcpSocket->close();
}
Web2TcpSocket::Web2TcpSocket(QTcpSocket *tcpSocket, const QUrl &webUrl, const QUrl &proxyUrl, QObject *parent):
    QObject(parent), m_tcpSocket(tcpSocket) {
    m_webSocket = new QWebSocket();
    m_webSocket->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyUrl.port(), proxyUrl.userName(), proxyUrl.password()));
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &Web2TcpSocket::tcpDisconnected);
    connect(m_webSocket, static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &Web2TcpSocket::webConnectionError);
    connect(m_webSocket, &QWebSocket::connected, this, &Web2TcpSocket::webConnected);
    m_webSocket->open(webUrl);
    qDebug() << "WebSocket Connexion at" << webUrl << "through" << proxyUrl;
}
void Web2TcpServer::tcpConnection() {
    new Web2TcpSocket(m_tcpServer.nextPendingConnection(), m_webUrl, m_proxyUrl, this);
}
Web2TcpServer::Web2TcpServer(const QUrl &webUrl, quint16 tcpPort, const QUrl &proxyUrl, QObject *parent):
    QObject(parent), m_webUrl(webUrl), m_proxyUrl(proxyUrl) {
    qDebug() << "Starting TCP server on port " << tcpPort;
    if (m_tcpServer.listen(QHostAddress::Any, tcpPort)) {
        connect(&m_tcpServer, &QTcpServer::newConnection, this, &Web2TcpServer::tcpConnection);
    } else {
        qWarning() << "Failed to open server on port " << tcpPort;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("WebSocket to TCP Socket bridge");
    parser.addHelpOption();
    QCommandLineOption webUrlOption(QStringList() << "u" << "webUrl",
            QCoreApplication::translate("main", "Web URL to connect to [default: ws://localhost:8080]."),
            QCoreApplication::translate("main", "webUrl"), QLatin1Literal("ws://localhost:8080"));
    parser.addOption(webUrlOption);
    QCommandLineOption tcpPortOption(QStringList() << "p" << "tcpPort",
            QCoreApplication::translate("main", "TCP port listening to [default: 2222]."),
            QCoreApplication::translate("main", "tcpPort"), QLatin1Literal("2222"));
    parser.addOption(tcpPortOption);
    QCommandLineOption proxyUrlOption(QStringList() << "P" << "proxyUrl",
            QCoreApplication::translate("main", "Web proxy server [default: $http_proxy]."),
            QCoreApplication::translate("main", "proxyUrl"), qgetenv("http_proxy"));
    parser.addOption(proxyUrlOption);
    parser.process(a);

    Web2TcpServer server(QUrl(parser.value(webUrlOption)), parser.value(tcpPortOption).toInt(), parser.value(proxyUrlOption));
    //QObject::connect(server, &Web2TcpServer::closed, &a, &QCoreApplication::quit);

    return a.exec();
}
