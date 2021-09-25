#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QTcpSocket>

class Tcp2WebSocket: public QObject
{
    Q_OBJECT
private:
    QWebSocket *m_webSocket;
    QTcpSocket *m_tcpSocket;
public:
    void webRead(const QByteArray &message);
    void tcpRead();
    void tcpConnected();
    void webDisconnected();
    void tcpConnectionError(QAbstractSocket::SocketError error);
    explicit Tcp2WebSocket(QWebSocket *webSocket, const QUrl &tcpUrl, QObject *parent=0);
};
class Tcp2WebServer: public QObject
{
    Q_OBJECT
private:
    QUrl m_tcpUrl;
    QWebSocketServer m_webServer;
private slots:
    void webConnection();
public:
    explicit Tcp2WebServer(const QUrl &tcpUrl, quint16 webPort, QObject *parent=0);
};
