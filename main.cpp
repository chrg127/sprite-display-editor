/*#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QString>
#include "main_window.h"
#include "tool.h"
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"*/
#include <iostream>
#include <iomanip>
#include "sprite.h"

#define DEBUG

#ifdef DEBUG
void test(void);
#endif

int main(int argc, char **argv)
{
    //QApplication a(argc, argv);
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
{/*
    QString mw2file = "test/mw2test";
    int err;

    smw::ROM rom;

    smw::openrom(&rom, "test/rom.smc", false);
    romutils::check_sprite_extensions(rom);
    err = romutils::mw2_readfile(mw2file);
    if (err != 0)
        qDebug() << "Received error:" << err;
    smw::closerom(&rom, false);*/
}
#endif

