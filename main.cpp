#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QString>
#include "main_window.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    //MainWindow window;

    a.setWindowIcon(QIcon("./stuff/icon64.png"));
    //window.show();
    return a.exec();
}
