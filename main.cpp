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
    QString mw2file = "test/rom";
    int err;
    QMultiMap<sprite::SpriteKey, sprite::SpriteValue> sprite_map;
    smw::ROM rom;

    smw::openrom(&rom, "test/rom.smc", false);
    sprite::load_size_table(rom);
    err = ssc_readfile(sprite_map, mw2file);
    if (err != 0)
        //qDebug() << "Received error:" << err << "while reading ssc file";
    sprite::print_sprites(sprite_map);
    mw2_writefile(sprite_map, "test/mw2out");
    mwt_writefile(sprite_map, "test/mwtout");
    ssc_writefile(sprite_map, "test/sscout");
    sprite_map.clear();
    smw::closerom(&rom, false);
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
    //window.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    window.show();

    return a.exec();
#endif
}


