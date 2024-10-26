// LabServer.h
#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QDate>
#include <QVector>

struct StudentRecord {
    QString name;
    QString rollNumber;
    QMap<QDate, QString> progressMessages;
};

class LabServer : public QObject {
    Q_OBJECT

public:
    explicit LabServer(QObject *parent = nullptr);

    bool startServer(int port);
    void stopServer();
    bool addStudent(const QString& rollNumber, const QString& name);
    bool importStudentsFromCSV(const QString& filePath);
    QVector<StudentRecord> getRegisteredStudents() const;
    QString getStudentProgress(const QString& rollNumber, const QDate& date) const;

signals:
    void clientConnected(const QString& rollNumber, const QString& name);
    void clientDisconnected(const QString& rollNumber);
    void progressUpdated(const QString& rollNumber, const QString& progress);
    void errorOccurred(const QString& error);

private slots:
    void handleNewConnection();
    void handleClientDisconnected();
    void handleClientData();

private:
    QTcpServer _server;
    QMap<QString, StudentRecord> _registeredStudents;  // rollNumber -> StudentRecord
    QMap<QTcpSocket*, QString> _connectedClients;      // socket -> rollNumber

    bool validateLogin(const QString& rollNumber, const QString& name);
    void processLoginRequest(QTcpSocket* socket, const QString& data);
    void processProgressUpdate(QTcpSocket* socket, const QString& data);
};
