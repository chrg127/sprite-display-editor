//#define DEBUG

#include <QApplication>
#include <QSizePolicy>
#include "main_window.h"
#include "tool.h"

#ifdef DEBUG
#include <QDebug>
#include <QString>
#include <iostream>
#include <iomanip>
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"
#include "sprite.h"
#include "sprite_files.h"

#include <QFile>

void test(void)
{
}
#endif

int main(int argc, char **argv)
{
#ifdef DEBUG
    test();
#else
    QApplication a(argc, argv);
    Tool tool;
    MainWindow window(&tool);
    a.setWindowIcon(QIcon("./stuff/icon64.png"));
    window.show();
    return a.exec();
#endif
}


