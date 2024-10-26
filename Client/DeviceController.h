#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H
#include <QTcpSocket>
#include <QObject>

class DeviceController : public QObject
{
    Q_OBJECT
public:
    explicit DeviceController(QObject *parent = nullptr);
    void connectToDevice(QString ip, int port);
    void sendLoginData(QString rollnumber,QString name);
    QAbstractSocket::SocketState state();
    void disconnect();
    bool isConnected();
    void send(QString);
signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState);
    void errorOccurred(QAbstractSocket::SocketError);
    void dataReady(QByteArray);
private slots:
    void socket_stateChanged(QAbstractSocket::SocketState state);
    void socket_readyRead();
private:
    QTcpSocket _socket;
    QString _ip;
    int _port;
};

#endif // DEVICECONTROLLER_H
