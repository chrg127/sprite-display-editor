#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <iostream>
#include <iomanip>
#include "main_window.h"
#include "tool.h"

#define DEBUG

#ifdef DEBUG
void test(void);
#endif

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    //MainWindow window;

    //a.setWindowIcon(QIcon("./stuff/icon64.png"));
    //window.show();
#ifdef DEBUG
    test();
#endif
    
#ifndef DEBUG
    return a.exec();
#endif
}

#ifdef DEBUG
void test(void)
{
    QString mw2file = "test/mw2test";
    int err;

    err = romutils::read_mw2_file(mw2file);
    qDebug() << "err:" << err;   
}
#endif

