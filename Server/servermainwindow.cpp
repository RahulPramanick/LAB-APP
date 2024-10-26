#include "servermainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>

LabServerMainWindow::LabServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    connect(&_server, &LabServer::clientConnected,
            this, &LabServerMainWindow::handleClientConnected);
    connect(&_server, &LabServer::clientDisconnected,
            this, &LabServerMainWindow::handleClientDisconnected);
    connect(&_server, &LabServer::progressUpdated,
            this, &LabServerMainWindow::handleProgressUpdate);
}

void LabServerMainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // Main background color
    centralWidget->setStyleSheet("background-color: rgb(198, 216, 255);");
    // Server control group
    QGroupBox* serverGroup = new QGroupBox("Server Control");
    QHBoxLayout* serverLayout = new QHBoxLayout(serverGroup);
    serverLayout->setSpacing(10);

    _portInput = new QSpinBox;
    _portInput->setRange(1024, 65535);
    _portInput->setValue(12345);
    _portInput->setFixedWidth(80);

    _startStopButton = new QPushButton("Start Server");
    setButtonStyles(_startStopButton);

    serverLayout->addWidget(new QLabel("Port:"));
    serverLayout->addWidget(_portInput);
    serverLayout->addStretch();
    serverLayout->addWidget(_startStopButton);

    // Student registration group
    QGroupBox* registrationGroup = new QGroupBox("Student Registration");
    QGridLayout* regLayout = new QGridLayout(registrationGroup);
    regLayout->setSpacing(10);

    _rollNumberInput = new QLineEdit;
    _nameInput = new QLineEdit;
    _rollNumberInput->setPlaceholderText("Enter Roll Number");
    _nameInput->setPlaceholderText("Enter Name");

    QPushButton* addButton = new QPushButton("Add Student");
    QPushButton* importButton = new QPushButton("Import from CSV");
    _exportButton = new QPushButton("Export to CSV");

    setButtonStyles(addButton);
    setButtonStyles(importButton);
    setButtonStyles(_exportButton);

    regLayout->addWidget(new QLabel("Roll Number:"), 0, 0);
    regLayout->addWidget(_rollNumberInput, 0, 1);
    regLayout->addWidget(new QLabel("Name:"), 1, 0);
    regLayout->addWidget(_nameInput, 1, 1);
    regLayout->addWidget(addButton, 2, 0);
    regLayout->addWidget(importButton, 2, 1);
    regLayout->addWidget(_exportButton, 3, 0, 1, 2);

    // Student table
    _studentTable = new QTableWidget;
    _studentTable->setColumnCount(4);
    _studentTable->setHorizontalHeaderLabels({"Roll Number", "Name", "Status", "Progress"});
    _studentTable->setAlternatingRowColors(true);
    _studentTable->setWordWrap(true);
    _studentTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _studentTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _studentTable->setMinimumHeight(200);

    _studentTable->setColumnWidth(0, 100);
    _studentTable->setColumnWidth(1, 200);
    _studentTable->setColumnWidth(2, 150);
    _studentTable->setColumnWidth(3, 300);
    _studentTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    _studentTable->resizeRowsToContents();
    _studentTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Add the components to the main layout
    mainLayout->addWidget(serverGroup);
    mainLayout->addWidget(registrationGroup);
    mainLayout->addWidget(_studentTable);

    // Set the initial size of the main window
    resize(800, 600);

    // Apply additional styles to components
    QString styleSheet = R"(
        QWidget {
            font-family: 'Arial';
            font-size: 12px;
            color: black;
        }
        QGroupBox {
            border: 2px solid rgb(107, 92, 165);
            border-radius: 5px;
            margin-bottom: 15px;
            padding: 10px;
            background-color: white;
            font-weight: bold;
        }
        QLabel {
            color: black;
            font-weight: bold;
        }
        QLineEdit, QSpinBox {
            background-color: white;
            border: 1px solid rgb(107, 92, 165);
            border-radius: 3px;
            padding: 4px;
            color: black;
        }
        QTableWidget {
            border: 2px solid rgb(107, 92, 165);
            background-color: white;
            gridline-color: rgb(113, 169, 247);
        }
        QTableWidget::item {
            color: black;
        }
        QHeaderView::section {
            background-color: rgb(113, 169, 247);
            color: black;
            font-weight: bold;
            padding: 6px;
            border: 1px solid rgb(107, 92, 165);
        }
        QTableWidget::item:alternate {
            background-color: rgb(232, 240, 255);
        }
    )";
    setStyleSheet(styleSheet);

    // Connect signals
    connect(_startStopButton, &QPushButton::clicked, this, &LabServerMainWindow::startStopServer);
    connect(addButton, &QPushButton::clicked, this, &LabServerMainWindow::addStudentManually);
    connect(importButton, &QPushButton::clicked, this, &LabServerMainWindow::importStudentsFromFile);
    connect(_exportButton, &QPushButton::clicked, this, &LabServerMainWindow::exportToCSV);
}

void LabServerMainWindow::setButtonStyles(QPushButton* button) {
    button->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                      stop:0 rgb(113, 169, 247),
                                      stop:1 rgb(107, 92, 165));
            color: black;
            font-weight: bold;
            border: none;
            padding: 8px;
            border-radius: 5px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                      stop:0 rgb(68, 255, 210),
                                      stop:1 rgb(113, 169, 247));
        }
        QPushButton:pressed {
            background: rgb(68, 255, 210);
            padding: 9px 7px 7px 9px;
        }
        QPushButton:disabled {
            background: rgb(200, 200, 200);
            color: rgb(120, 120, 120);
        }
    )");
}
void LabServerMainWindow::exportToCSV() {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    QString defaultFilename = QString("student_progress_%1.csv").arg(timestamp);

    QString filePath = QFileDialog::getSaveFileName(this,
                                                    "Export Student Progress",
                                                    defaultFilename,
                                                    "CSV Files (*.csv)");

    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error",
                             "Failed to open file for writing: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // Write header
    out << "Roll Number,Name,Status,Progress\n";

    // Write data from table
    for (int row = 0; row < _studentTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < _studentTable->columnCount(); ++col) {
            QTableWidgetItem* item = _studentTable->item(row, col);
            QString cellData = item ? item->text() : "";

            // If it's the progress column, append the attendance status
            if (col == 3) {
                QString rollNumber = _studentTable->item(row, 0)->text();
                QString attendanceStatus = _presentStudents.contains(rollNumber) ? "Present" : "Absent";
                cellData.append(" - " + attendanceStatus);  // Append status to progress
            }

            // Escape commas and quotes in the data
            if (cellData.contains(',') || cellData.contains('"')) {
                cellData = "\"" + cellData.replace("\"", "\"\"") + "\"";
            }
            rowData << cellData;
        }
        out << rowData.join(",") << "\n";
    }

    file.close();

    QMessageBox::information(this, "Success",
                             "Student progress has been exported to:\n" + filePath);
}


void LabServerMainWindow::startStopServer() {
    if (_startStopButton->text() == "Start Server") {
        if (_server.startServer(_portInput->value())) {
            _startStopButton->setText("Stop Server");
            _portInput->setEnabled(false);
            _isSessionActive = true;  // Start the session
            _presentStudents.clear();  // Clear previous session's present students
        }
    } else {
        _isSessionActive = false;  // End the session
        _server.stopServer();
        _startStopButton->setText("Start Server");
        _portInput->setEnabled(true);
        updateStudentTable();  // Update table to show attendance
    }
}


void LabServerMainWindow::addStudentManually() {
    QString rollNumber = _rollNumberInput->text().trimmed();
    QString name = _nameInput->text().trimmed();

    if (rollNumber.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both roll number and name");
        return;
    }

    if (_server.addStudent(rollNumber, name)) {
        updateStudentTable();
        _rollNumberInput->clear();
        _nameInput->clear();
    } else {
        QMessageBox::warning(this, "Error", "Student already registered");
    }
}

void LabServerMainWindow::importStudentsFromFile() {
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "Import Students", "", "CSV Files (*.csv)");

    if (!filePath.isEmpty() && _server.importStudentsFromCSV(filePath)) {
        updateStudentTable();
    }
}

void LabServerMainWindow::updateStudentTable() {
    _studentTable->setRowCount(0);
    auto students = _server.getRegisteredStudents();

    for (const auto& student : students) {
        int row = _studentTable->rowCount();
        _studentTable->insertRow(row);

        _studentTable->setItem(row, 0, new QTableWidgetItem(student.rollNumber));
        _studentTable->setItem(row, 1, new QTableWidgetItem(student.name));

        // Set the status based on whether the student was present in this session
        QString status = _presentStudents.contains(student.rollNumber) ? "Present" : "Absent";
        _studentTable->setItem(row, 2, new QTableWidgetItem(status));

        // Set the progress messages
        _studentTable->setItem(row, 3,
                               new QTableWidgetItem(student.progressMessages.value(QDate::currentDate())));
    }
}


void LabServerMainWindow::handleClientConnected(
    const QString& rollNumber, const QString& name) {
    if (_isSessionActive) {
        _presentStudents.insert(rollNumber);  // Mark student as present
    }
    updateStudentStatus(rollNumber, true);
}

void LabServerMainWindow::handleClientDisconnected(const QString& rollNumber) {
    updateStudentStatus(rollNumber, false);
}


void LabServerMainWindow::handleProgressUpdate(
    const QString& rollNumber, const QString& progress) {
    for (int row = 0; row < _studentTable->rowCount(); ++row) {
        if (_studentTable->item(row, 0)->text() == rollNumber) {
            _studentTable->setItem(row, 3, new QTableWidgetItem(progress));
            break;
        }
    }
}

void LabServerMainWindow::updateStudentStatus(
    const QString& rollNumber, bool connected) {
    for (int row = 0; row < _studentTable->rowCount(); ++row) {
        if (_studentTable->item(row, 0)->text() == rollNumber) {
            _studentTable->setItem(row, 2,
                                   new QTableWidgetItem(connected ? "Connected" : "Disconnected"));
            break;
        }
    }
}
