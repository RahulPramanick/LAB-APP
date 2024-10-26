// LabServer.cpp
#include "LabServer.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

LabServer::LabServer(QObject *parent) : QObject(parent) {
    connect(&_server, &QTcpServer::newConnection,
            this, &LabServer::handleNewConnection);
}

bool LabServer::startServer(int port) {
    if (_server.listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
        return true;
    }
    emit errorOccurred("Failed to start server: " + _server.errorString());
    return false;
}

void LabServer::stopServer() {
    _server.close();
    for (auto socket : _connectedClients.keys()) {
        socket->close();
    }
    _connectedClients.clear();
}

bool LabServer::addStudent(const QString& rollNumber, const QString& name) {
    if (_registeredStudents.contains(rollNumber)) {
        return false;
    }

    StudentRecord record;
    record.rollNumber = rollNumber;
    record.name = name;
    _registeredStudents[rollNumber] = record;
    return true;
}

bool LabServer::importStudentsFromCSV(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("Failed to open CSV file");
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() >= 2) {
            addStudent(fields[0].trimmed(), fields[1].trimmed());
        }
    }
    return true;
}

void LabServer::handleNewConnection() {
    QTcpSocket* clientSocket = _server.nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected,
            this, &LabServer::handleClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead,
            this, &LabServer::handleClientData);
}

void LabServer::handleClientDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QString rollNumber = _connectedClients.value(socket);
        if (!rollNumber.isEmpty()) {
            emit clientDisconnected(rollNumber);
        }
        _connectedClients.remove(socket);
        socket->deleteLater();
    }
}

void LabServer::handleClientData() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString data = QString::fromUtf8(socket->readAll());

    if (data.startsWith("LOGIN:")) {
        processLoginRequest(socket, data);
    } else {
        processProgressUpdate(socket, data);
    }
}

bool LabServer::validateLogin(const QString& rollNumber, const QString& name) {
    if (_registeredStudents.contains(rollNumber)) {
        const StudentRecord& record = _registeredStudents[rollNumber];
        return record.name.toLower() == name.toLower();
    }
    return false;
}

void LabServer::processLoginRequest(QTcpSocket* socket, const QString& data) {
    QStringList lines = data.split('\n');
    QString rollNumber, name;

    for (const QString& line : lines) {
        if (line.startsWith("Roll:")) {
            rollNumber = line.mid(5).trimmed();
        } else if (line.startsWith("Name:")) {
            name = line.mid(5).trimmed();
        }
    }

    if (validateLogin(rollNumber, name)) {
        _connectedClients[socket] = rollNumber;
        socket->write("Login successful\n");
        emit clientConnected(rollNumber, name);
    } else {
        socket->write("Access denied\n");
        socket->disconnectFromHost();
    }
}

void LabServer::processProgressUpdate(QTcpSocket* socket, const QString& data) {
    QString rollNumber = _connectedClients.value(socket);
    if (rollNumber.isEmpty()) return;

    if (_registeredStudents.contains(rollNumber)) {
        StudentRecord& record = _registeredStudents[rollNumber];
        record.progressMessages[QDate::currentDate()] = data;
        emit progressUpdated(rollNumber, data);
    }
}

QVector<StudentRecord> LabServer::getRegisteredStudents() const {
    return QVector<StudentRecord>::fromList(_registeredStudents.values());
}

QString LabServer::getStudentProgress(const QString& rollNumber, const QDate& date) const {
    if (_registeredStudents.contains(rollNumber)) {
        const StudentRecord& record = _registeredStudents[rollNumber];
        return record.progressMessages.value(date);
    }
    return QString();
}
