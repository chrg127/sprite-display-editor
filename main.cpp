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

    smw::ROM rom;

    smw::openrom(&rom, "test/rom.smc", false);
    sprite::load_size_table(rom);
    err = romutils::mw2_mwt_readfile(mw2file);
    if (err != 0)
        qDebug() << "Received error:" << err;
    smw::closerom(&rom, false);
}
#endif

