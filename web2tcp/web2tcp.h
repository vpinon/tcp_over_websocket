#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtWebSockets/QWebSocket>

class Web2TcpSocket: public QObject
{
    Q_OBJECT
private:
    QTcpSocket *m_tcpSocket;
    QWebSocket *m_webSocket;
public:
    void tcpRead();
    void webRead(const QByteArray &message);
    void webConnected();
    void tcpDisconnected();
    void webConnectionError(QAbstractSocket::SocketError error);
    explicit Web2TcpSocket(QTcpSocket *tcpSocket, const QUrl &webUrl, const QUrl &proxyUrl, QObject *parent=0);
};
class Web2TcpServer: public QObject
{
    Q_OBJECT
private:
    QUrl m_webUrl;
    QUrl m_proxyUrl;
    QTcpServer m_tcpServer;
private slots:
    void tcpConnection();
public:
    explicit Web2TcpServer(const QUrl &webUrl, quint16 tcpPort, const QUrl &proxyUrl, QObject *parent=0);
};
