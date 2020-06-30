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
    for (int i = 0; i < 0x2000; i++) {
        std::printf("num: %X\n", i);
        for (int j = 0; j < 4; j++) {
            std::printf("tile %d\n", j);
            std::printf("offset: %X\n", arr[i].tile8[j].offset);
            std::printf("x: %X, y: %X\n", arr[i].tile8[j].x, arr[i].tile8[j].y);
            std::printf("priority: %X\n", arr[i].tile8[j].priority);
            std::printf("pal: %X\n", arr[i].tile8[j].pal);
            std::printf("tt: %X\n", arr[i].tile8[j].tt);
            std::printf("\n");
        }
        std::printf("\n");
    }

    return 0;
}

