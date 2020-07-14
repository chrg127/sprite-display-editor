#include <QApplication>
#include <QDebug>
/*#include "main_window.h"
#include "tool.h"
#include "sprite.h"*/
#include "ext/libsmw.h"
#include "palette.h"

int main(int argc, char **argv)
{
/*    QApplication a(argc, argv);
    Tool tool;
    MainWindow window(&tool);
    a.setWindowIcon(QIcon("./stuff/icon64.png"));
    window.show();
    return a.exec();*/
    char romname[1024] = "./test/rom.smc";
    smw::ROM rom;

    rom.openrom(romname, false);
    SNESColor paltable[8192];
    get_global_sprite_palette(rom, paltable, 0);
    for (int i = 0; i < 48; i++)
        paltable[i].print();
    rom.closerom(false);

    return 0;
}

