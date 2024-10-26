// Main.cpp
#include <QApplication>
#include "servermainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    LabServerMainWindow window;
    window.show();
    return app.exec();
}
