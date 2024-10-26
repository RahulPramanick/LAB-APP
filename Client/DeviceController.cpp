#include "DeviceController.h"

DeviceController::DeviceController(QObject *parent)
    : QObject{parent}
{
    connect(&_socket, &QTcpSocket::connected, this, &DeviceController::connected);
    connect(&_socket, &QTcpSocket::disconnected, this, &DeviceController::disconnected);
    connect(&_socket, &QTcpSocket::stateChanged, this, &DeviceController::socket_stateChanged);
    connect(&_socket, &QTcpSocket::errorOccurred, this, &DeviceController::errorOccurred);
    connect(&_socket, &QTcpSocket::readyRead, this, &DeviceController::socket_readyRead);
}

void DeviceController::connectToDevice(QString ip, int port)
{
    if (_socket.isOpen()) {
        if (ip == _ip && port == _port) {
            return;
        }
        _socket.close();
    }
    _ip = ip;
    _port = port;
    _socket.connectToHost(_ip, _port);
}

void DeviceController::sendLoginData(QString rollnumber, QString name)
{
    // Format the login data as a message to be sent to the server
    QString loginData = QString("LOGIN:\nRoll:%1\nName:%2\n").arg(rollnumber).arg(name);

    // Send the data to the server using the socket
    _socket.write(loginData.toUtf8());
}

QAbstractSocket::SocketState DeviceController::state()
{
    return _socket.state();
}

void DeviceController::disconnect()
{
    _socket.close();
}

bool DeviceController::isConnected()
{
    return _socket.state() == QAbstractSocket::ConnectedState;
}

void DeviceController::send(QString message)
{
    _socket.write(message.toUtf8());
}

void DeviceController::socket_stateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState) {
        _socket.close();
    }
    emit stateChanged(state);
}

void DeviceController::socket_readyRead()
{
    // Read data from the socket
    auto data = _socket.readAll();
    emit dataReady(data);

    // Handle the server response
    QString response(data);
    if (response.contains("Access denied")) {
        // Handle the rejection
        qDebug() << "Access denied. Please contact your lab attendant.";
    } else if (response.contains("Login successful")) {
        // Proceed with further actions
        qDebug() << "Login successful.";
    }
}
