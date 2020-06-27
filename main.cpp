#include <QApplication>
#include <QDebug>
/*#include "main_window.h"
#include "tool.h"
#include "sprite.h"*/
#include "file_formats.h"
#include "map16.h"

int main(int argc, char **argv)
{
/*    QApplication a(argc, argv);
    Tool tool;
    MainWindow window(&tool);
    a.setWindowIcon(QIcon("./stuff/icon64.png"));
    window.show();
    return a.exec();*/
    char romname[1024] = "./test/rom";
    Maptile arr[0x2000];
    int ret = s16_readfile(arr, romname);
    qDebug() << ret;
}

