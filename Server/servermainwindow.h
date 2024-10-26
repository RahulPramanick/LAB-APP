// LabServerMainWindow.h
#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include "LabServer.h"

class LabServerMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LabServerMainWindow(QWidget *parent = nullptr);
private slots:
    void startStopServer();
    void addStudentManually();
    void importStudentsFromFile();
    void updateStudentTable();
    void handleClientConnected(const QString& rollNumber, const QString& name);
    void handleClientDisconnected(const QString& rollNumber);
    void handleProgressUpdate(const QString& rollNumber, const QString& progress);
    void exportToCSV();  // New slot for export functionality

private:
    LabServer _server;
    QTableWidget* _studentTable;
    QPushButton* _startStopButton;
    QLineEdit* _rollNumberInput;
    QLineEdit* _nameInput;
    QSpinBox* _portInput;
    QPushButton* _exportButton;  // New button for export
    bool _isSessionActive;  // Track whether the session is active
    QSet<QString> _presentStudents;  // Track roll numbers of present students

    void setupUI();
    void updateStudentStatus(const QString& rollNumber, bool connected);
    void setButtonStyles(QPushButton *button);
};
