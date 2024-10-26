#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QStyle>
#include <QMetaEnum>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setDeviceController();
    setupAssignment();
    ui->assignmentSubPart->setVisible(false);
    ui->label_7->setVisible(false);
    _subPartsDoneCompleteMessage.clear(); // Initialize here
    _subPartsDoneCompleteMessage.clear(); // Initialize here
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupAssignment() {
    ui->assignments->clear();

    // Add items to the combo box with color coding
    ui->assignments->addItem("1. Raster Grid with Quadrants");
    ui->assignments->addItem("2. Line Drawing Algorithms");
    ui->assignments->addItem("3. Circle Drawing Algorithms");
    ui->assignments->addItem("4. Ellipse Drawing Algorithms");
    ui->assignments->addItem("5. Closed Polygon with Scanline Fill");
    ui->assignments->addItem("6. Seed-Fill Algorithms");
    ui->assignments->addItem("7. Polygon Transformations");
    ui->assignments->addItem("8. Composite Transformations");
    ui->assignments->addItem("9. Line Clipping");
    ui->assignments->addItem("10. Polygon Clipping");
    ui->assignments->addItem("11. Bezier Curve Implementation");
    ui->assignments->addItem("None");

    ui->assignments->setCurrentIndex(12);

    // Set the color for optional items
    ui->assignments->setItemData(2, QColor(127, 255, 127, 100), Qt::BackgroundRole);
    ui->assignments->setItemData(8, QColor(127, 255, 127, 100), Qt::BackgroundRole);
    ui->assignments->setItemData(9, QColor(127, 255, 127, 100), Qt::BackgroundRole);
    ui->assignments->setItemData(10, QColor(0, 255, 0, 100), Qt::BackgroundRole);
}

void MainWindow::setupAssignmentSubPart(int index) {
    // Clear the existing items in the combo box
    ui->assignmentSubPart->clear();

    // Populate the subpart combo box based on the selected assignment
    switch (index) {
    case 0: // 1. Raster Grid with Quadrants
        ui->assignmentSubPart->addItem("Raster Grid");
        break;
    case 1: // 2. Line Drawing Algorithms
        ui->assignmentSubPart->addItems({"DDA", "Bresenham Line"});
        break;
    case 2: // 3. Circle Drawing Algorithms
        ui->assignmentSubPart->addItems({"Polar Circle", "Bresenham Circle", "Cartesian", "Midpoint"});
        ui->assignmentSubPart->setItemData(3, QColor(127, 255, 127, 100), Qt::BackgroundRole); // Light Green for optional
        break;
    case 3: // 4. Ellipse Drawing Algorithms
        ui->assignmentSubPart->addItems({"Polar Ellipse", "Bresenham Ellipse"});
        break;
    case 4: // 5. Closed Polygon with Scanline Fill
        ui->assignmentSubPart->addItem("Scanline Fill");
        break;
    case 5: // 6. Seed-Fill Algorithms
        ui->assignmentSubPart->addItems({"Boundary Fill", "Flood Fill - 4 Connected", "Flood Fill - 8 Connected"});
        break;
    case 6: // 7. Polygon Transformations
        ui->assignmentSubPart->addItems({"Translation", "Rotation", "Scaling", "Shear", "Reflection"});
        break;
    case 7: // 8. Composite Transformations
        ui->assignmentSubPart->addItems({"Rotation wrt Arbitrary Point","Scaling wrt Arbitrary Point", "Reflection wrt Arbitrary Line"});
        break;
    case 8: // 9. Line Clipping
        ui->assignmentSubPart->addItems({"Cohen-Sutherland", "Liang-Barsky (Optional)"});
        ui->assignmentSubPart->setItemData(1, QColor(127, 255, 127, 100), Qt::BackgroundRole); // Light Green for optional
        break;
    case 9: // 10. Polygon Clipping
        ui->assignmentSubPart->addItems({"Sutherland-Hodgeman", "Weiler-Atherton (Optional)"});
        ui->assignmentSubPart->setItemData(1, QColor(127, 255, 127, 100), Qt::BackgroundRole); // Light Green for optional
        break;
    case 10: // 11. Bezier Curve Implementation
        ui->assignmentSubPart->addItem("Bezier Curve");
        break;
    default:
        ui->assignmentSubPart->addItem("None");
        break;
    }
}

void MainWindow::on_ipAddress_textChanged(const QString &arg1)
{
    QString state = "0";
    if (arg1 == "...") {
        state = "";
    } else {
        QHostAddress address(arg1);
        if (QAbstractSocket::IPv4Protocol == address.protocol()) {
            state = "1";
        }
    }
    ui->ipAddress->setProperty("state", state);
    style()->polish(ui->ipAddress);
}

void MainWindow::on_btnConnect_clicked()
{
    if (_controller.isConnected()) {
        _controller.disconnect();
    } else {
        auto rollnumber = ui->rollNum->text().trimmed();
        auto name = ui->name->text().trimmed();

        if (rollnumber.isEmpty()) {
            ui->rollNum->setPlaceholderText("Enter Roll Number");
            return;
        }

        if (name.isEmpty()) {
            ui->name->setPlaceholderText("Enter Name");
            return;
        }

        auto ip = ui->ipAddress->text();
        auto port = ui->portNum->value();
        _controller.connectToDevice(ip, port);
        _controller.sendLoginData(rollnumber, name);
        _subPartsDoneCompleteMessage.clear(); // Initialize here
    }
}

void MainWindow::device_connected()
{
    ui->terminal->append("\nConnected to a Device\n");
    ui->btnConnect->setText("Disconnect");
    ui->grpSendData->setEnabled(true);
}

void MainWindow::device_disconnected()
{
    ui->terminal->append("\nDisconnected from a Device");
    ui->btnConnect->setText("Connect");
    ui->grpSendData->setEnabled(false);
}

void MainWindow::device_stateChanged(QAbstractSocket::SocketState state)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    ui->terminal->append(metaEnum.valueToKey(state));
}

void MainWindow::device_errorOccurred(QAbstractSocket::SocketError error)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    ui->terminal->append(metaEnum.valueToKey(error));
}

void MainWindow::device_dataReady(QByteArray data)
{
    ui->terminal->append("\n"+QString(data));
}

void MainWindow::setDeviceController()
{
    connect(&_controller, &DeviceController::connected, this, &MainWindow::device_connected);
    connect(&_controller, &DeviceController::disconnected, this, &MainWindow::device_disconnected);
    connect(&_controller, &DeviceController::stateChanged, this, &MainWindow::device_stateChanged);
    connect(&_controller, &DeviceController::errorOccurred, this, &MainWindow::device_errorOccurred);
    connect(&_controller, &DeviceController::dataReady, this, &MainWindow::device_dataReady);
}

void MainWindow::on_btnSend_clicked()
{
    // Send the combined message
    _controller.send(_subPartsDoneCompleteMessage);
}

void MainWindow::setupDifficultyCheckboxes(const QString &subpart) {
    // Clear existing checkboxes
    QLayoutItem* item;
    while ((item = ui->difficultyLayout->takeAt(0))) {
        delete item->widget(); // Delete the widget
        delete item; // Delete the layout item
    }

    // Create checkboxes based on the selected subpart's common mistakes
    QStringList mistakes = mistakeMap.value(subpart, QStringList());
    if(mistakes.count()>0){
        qDebug() << "Mistakes for subpart" << subpart << ":" << mistakes; // Debug output

        for (const QString& mistake : mistakes) {
            QCheckBox* checkBox = new QCheckBox(mistake);
            ui->difficultyLayout->addWidget(checkBox); // Add the checkbox to the layout
        }

        // Optional: Update the layout to reflect the new widgets
        ui->difficultyLayout->update(); // Update layout
    }
    else{
        ui->label_7->setVisible(false);
        ui->difficultyLayout->setEnabled(false);
    }
}



void MainWindow::on_assignments_currentIndexChanged(int index)
{
    ui->assignmentSubPart->setVisible(true);
    setupAssignmentSubPart(index);
    ui->message->setText("None");
}

void MainWindow::on_addPassedAssignments_clicked()
{
    // Get the selected assignment and subpart
    QString selectedAssignment = ui->assignments->currentText();
    QString selectedSubpart = ui->assignmentSubPart->currentText();
    QString issuesLeft=ui->message->text().trimmed();
    // Prepare to collect passed test cases
    QStringList passedTestCases;

    for (int i = 0; i < ui->difficultyLayout->count(); ++i) {
        QLayoutItem* item = ui->difficultyLayout->itemAt(i);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(item->widget());
        if (checkBox && checkBox->isChecked()) {
            qDebug() << checkBox->text(); // Debug output to check checkbox text
            passedTestCases.append(checkBox->text());
        }
    }

    // Create the message for the passed assignment
    QString message = QString("Assignment: %1, Subpart: %2, Passed Test Cases: %3, Issues: %4")
                          .arg(selectedAssignment)
                          .arg(selectedSubpart)
                          .arg(passedTestCases.join(", "))
                          .arg(issuesLeft);

    // Append the message to the subparts complete message
    _subPartsDoneCompleteMessage.append(message + "\n"); // Append newline for clarity

    // Optionally, you might want to display this message somewhere in the UI
    ui->terminal->append(message); // Just an example of displaying the message }
}

void MainWindow::on_assignmentSubPart_currentIndexChanged(int index)
{
    QString selectedSubpart = ui->assignmentSubPart->currentText();
    ui->label_7->setVisible(true);
    setupDifficultyCheckboxes(selectedSubpart);
    ui->message->setText("None");
};

