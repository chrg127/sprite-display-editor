/*
#include <QMainWindow>
#include <QDebug>
#include <QString>
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"*/
#include <QApplication>
#include <iostream>
#include <iomanip>
#include "sprite.h"
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
    QString mw2file = "test/random";
    int err;
    QMultiMap<sprite::SpriteKey, sprite::SpriteValue> sprite_map;

    smw::ROM rom;

    smw::openrom(&rom, "test/rom.smc", false);
    sprite::load_size_table(rom);
    err = romutils::mw2_mwt_readfile(sprite_map, mw2file);
    if (err != 0)
        qDebug() << "Received error:" << err;
    err = romutils::ssc_readfile(sprite_map, mw2file);
    if (err != 0)
        qDebug() << "Received error:" << err << "while reading ssc file";
    sprite::print_sprites(sprite_map);
    romutils::mw2_writefile(sprite_map, "test/mw2out");
    romutils::mwt_writefile(sprite_map, "test/mwtout");
    romutils::ssc_writefile(sprite_map, "test/sscout");
    sprite_map.clear();
    smw::closerom(&rom, false);
}
#endif

