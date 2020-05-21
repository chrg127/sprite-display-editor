#include "tool.h"

#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include <QStringList>
#include <QtGlobal>
#include <iostream>
#include <iomanip>
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"

#define PRINTHEX(v) \
    do { \
       std::cout << std::uppercase << std::hex << std::setw(2) \
              << std::setfill('0') << (int) (v); \
    } while(0);

namespace romutils {
/*
void generate_sprite_list(const QString &romname)
{
    //Given a filename without extension, open all these files:
    //romname.ssc
    //romname.mw2
    //romname.mwt
}
*/

/* Reads the .mw2 file. mw2 file format:
 * first byte: sprite data header, should always be 00
 * next is the list of all sprites. Each sprite is composed of 3 bytes:
 * yyyyEESY XXXXssss NNNNNNNN
 * yyyy - Y position
 * EE - Extra bits
 * XXXX - X position
 * ssss - Screen number
 * NNNNNNNN - Sprite ID (or command)
 * last byte: end of data. Should be 0xFF. */
int read_mw2_file(const QString &romname)
{   
    QFile mw2file(romname + ".mw2");
    QDataStream mw2stream;
    char *read_bytes;
    qint64 nread;
    int ret = 0;
    unsigned char ypos, xpos, extrabits, screennum, spriteid;
 
    if (!mw2file.exists())
        return 2;
    if (!mw2file.open(QIODevice::ReadOnly))
        return 1;
    
    mw2stream.setDevice(&mw2file);
    read_bytes = new char[4];
    // Read single byte header.
    nread = mw2stream.readRawData(read_bytes, 1);
    if (nread != 1 || *read_bytes != 0x00) {
        ret = 1;
        goto cleanup;
    }
    
    // Read each sprite's bytes until end of data has been found.
    while ((nread = mw2stream.readRawData(read_bytes, 3)) != 1 && 
            *read_bytes != (char) 0xFF) {
        if (nread != 3) {
            ret = 1;
            goto cleanup;
        }
        ypos = ((read_bytes[0] & 0xF0) >> 4) | ((read_bytes[0] & 1) << 4);
        extrabits = (read_bytes[0] & 0xC) >> 2;
        xpos = (read_bytes[1] & 0xF0) >> 4;
        screennum = (read_bytes[1] & 0xF) | ((read_bytes[0] & 2) << 3);
        spriteid = read_bytes[2];

        PRINTHEX(read_bytes[0]);
        std::cout << ' ';
        PRINTHEX(read_bytes[1]);
        std::cout << ' ';
        PRINTHEX(read_bytes[2]);
        std::cout << ' ';
        std::cout << '\n';
        std::cout << "spriteid: ";
        PRINTHEX(spriteid);
        std::cout << "; extrabits: ";
        PRINTHEX(extrabits);
        std::cout << "; ypos: ";
        PRINTHEX(ypos);
        std::cout << "; xpos: ";
        PRINTHEX(xpos);
        std::cout << "; screennum: ";
        PRINTHEX(screennum);
        std::cout << '\n';
    }
    
    // Check for errors with end of data byte.
    if (nread != 1 || *read_bytes != (char) 0xFF) {
        ret = 1;
        goto cleanup;
    }

cleanup:    
    delete read_bytes;
    return ret;
}

/* Checks if there are any sprite with extension bytes.
 * This is checked by reading $0EF30F and check if it's equal to 42. If 
 * it is, then the address to a table representing each sprite's data 
 * size is found at $0EF30F. Example
 * ... 30 D2 12 42 ...
 *     ^        ^
 *     address  byte check */
void check_sprite_extensions(LibSMW::SnesRom &rom)
{

}

/* Reads .mwt file line by line. Example line:
 * (cmd)\t(sprite name)\n
 * After splitting a line, it converts the command and finds the sprite 
 * using the command, then it will update its name. */
int read_mwt_file(const QString &romname)
{
    QFile mwtfile(romname + ".mwt");
    QTextStream mwtstream;
    QStringList strlist;
    unsigned char cmd;
    bool ok;

    if (!mwtfile.exists())
        return 2;
    if (!mwtfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;
 
    mwtstream.setDevice(&mwtfile);
    while (!mwtstream.atEnd()) {
        strlist = mwtstream.readLine().split('\t',
                QString::SkipEmptyParts);
        cmd = strlist[0].toInt(&ok, 16);
        if (!ok)
            return 1;
        spritelist[cmd].name = strlist[1];
    }
    mwtfile.close();
    return 0;
}

/* Reads .ssc file line by line. In an ssc file there can be two kind of 
 * lines:
 *  - (cmd)\t(type(20/30))\t(tooltip)\n
 *  - (cmd)\t(type(20/30))\t(spritetileinfo...)\n
 * On the first type, we just need to collect the tooltip.
 * On the second, we'll have to collect and insert every sprite tile.
 * Again, we have to get the command first to do any operation. */
int read_ssc_file(const QString &romname)
{
    QFile sscfile(romname + ".ssc");
    QTextStream sscstream;
    QStringList strlist;
    int ncolumns, err, nlines = 0;
    bool ok;
    SpriteID cmd;
    
    if (!sscfile.exists())
        return 2;
    if (!sscfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;
    
    sscstream.setDevice(&sscfile);
    while (!sscstream.atEnd()) {
        strlist = sscstream.readLine().split('\t',  // Read line
                QString::SkipEmptyParts);
        nlines++;
        cmd = strlist[0].toInt(&ok, 16);
        if (!ok) {
            qDebug() << "Error at line" << nlines 
                << ": couldn't convert cmd";
            return 1;
        }
        ncolumns = strlist.size();

        if (ncolumns < 3) {
            qDebug() << "Error at line" << nlines 
                << ": ncolumns less than 3";
            return 3;
        }
        if (ncolumns == 3 && strlist[2][0].isDigit()) // Tooltip
            spritelist[cmd].tooltip = strlist[2];
        else {  // Sprite tiles
            strlist.removeAt(0);    // Remove cmd and extrabytes
            strlist.removeAt(1);
            err = spritelist[cmd].parse_add_tile(strlist);
            if (err != 0) {
                qDebug() << "Error at line" << nlines 
                         << ": not a spritetile string";
            }
        }
    }
    sscfile.close();

    return 0;
}


}

