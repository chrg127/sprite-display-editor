/* Custom sprite data is split into 3 files: the mw2 file, the mwt file and the ssc file.
 * The mw2 consists of the main sprite data: this is parsed first
 * */

/* The mw2 file format is the same as the sprite data in a SMW level. The
 * first byte consists of sprite data header, which should always be 00.
 * Next is the list of all sprites. Each sprite is usually composed of 3 bytes:
 * yyyyEESY XXXXssss NNNNNNNN
 * yyyy - Y position
 * EE - Extra bits
 * XXXX - X position
 * ssss - Screen number
 * NNNNNNNN - Sprite ID (or command)
 * last byte: end of data. Should be 0xFF.
 * The size of the sprite data can change, as explained earlier. For this
 * reason, before parsing this data we must find the size of each sprite.
 * No field but the sprite's ID and the extra bits interest us, since 
 * they only tell about the position the sprite was when it was added 
 * to the custom list. It would probably be more interesting had this 
 * been a level parser. */

/* This is how you get the other fields, anyway:
 * ypos = ((read_bytes[0] & 0xF0) >> 4) | ((read_bytes[0] & 1) << 4);
 * xpos = (read_bytes[1] & 0xF0) >> 4;
 * screennum = (read_bytes[1] & 0xF) | ((read_bytes[0] & 2) << 3); 
 */

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
#include "sprite.h"

#ifndef DEBUG
#define DEBUG
#endif

#ifdef DEBUG
#define PRINTHEX(v) \
    do { \
       std::cout << std::uppercase << std::hex << std::setw(2) \
              << std::setfill('0') << (int) (v); \
    } while(0);
#endif

namespace romutils {


/* Reads the next sprite bytes from stream and fills sp.
 * Return 1 for error, 2 for end of data byte. */
static int mw2_parse(QDataStream &mw2stream, sprite::SpriteKey &spk, sprite::SpriteValue &spv,
        char *read_bytes)
{
    qint64 nread, toread;
    int i;
    
    if (mw2stream.atEnd())
        return 0;

    // Read first 3 bytes and get the sprite's ID and extra bits.
    nread = mw2stream.readRawData(read_bytes, SPRITE_DEF_DATA_SIZE);
    if (nread == 1 && *read_bytes == (char) 0xFF)
        return 2;
    if (nread != SPRITE_DEF_DATA_SIZE) {
#ifdef DEBUG
        qDebug() << "Error while reading first 3 sprite bytes."
                 << "mw2stream state:" << mw2stream.atEnd();
#endif
        return 1;
    }
    spk.id = read_bytes[2];
    spk.extra_bits((read_bytes[0] & 0xC) >> 2);

    // Read extension bytes
    toread = spk.get_ext_size();
    if (toread <= 0)
        return 0;
    nread = mw2stream.readRawData(read_bytes, toread);
    if (toread != nread) {
#ifdef DEBUG
        qDebug() << "Error while reading extension bytes.";
#endif
        return 1;
    }
    for (i = 0; i < nread; i++)
        spv.ext_bytes[i] = read_bytes[i];

    return 0;
}

/* Parses a single line from the mwt file. Returns 1 on error.
 * Line format: (sprite id)\t(sprite name)\n
 * The sprite ID can be omitted. If it's present, the function returns 2,
 * otherwise returns 0 (to mean: if ret == 0, do no error checking on id) */
static int mwt_parse(QTextStream &mwtstream, unsigned char &id, QString &name)
{
    QStringList strlist;
    int name_col = 0, ret = 0;
    bool ok;

    strlist = mwtstream.readLine().split('\t', QString::SkipEmptyParts);
    if (strlist.size() > 2 || strlist.size() == 0) {
#ifdef DEBUG
        qDebug() << "ERROR: either 1 or 2 columns allowed";
#endif
        return 1;
    }
    if (strlist.size() == 2) {
        id = strlist[0].toInt(&ok, 16);
        if (!ok)
            return 1;
        name_col = 1;
        ret = 2;
    }
    name = strlist[name_col];
    return ret;
}

/* Reads the mw2 file associated with the ROM. romname must be the full
 * or relative path to the ROM, without extension.
 * Using this function is how sprites are added to the global spritelist
 * data structure: this is where sprite ID, extra bits and extension bytes
 * are found.
 * This function SHOULD be called after check_sprite_extensions. It is
 * not necessary to do so, but read errors will happen if not done so. 
 *
 * Returns 1 for any file error, 2 for byte header error, 3 for error
 * reading sprite data, 4 for end of file byte error */
/* Reads .mwt file line by line.Example line:
 * (cmd)\t(sprite name)\n 
 * After splitting a line, it converts the command and finds the sprite 
 * using the command, then it will update its name. */
int mw2_mwt_readfile(const QString &romname)
{
    QFile mw2file(romname + ".mw2"), mwtfile(romname + ".mwt");
    QDataStream mw2stream;
    QTextStream mwtstream;
    char *read_bytes;
    int ret = 0, parseret;
    unsigned char mwt_id;
    QString mwt_name;
    sprite::SpriteKey spk;
    sprite::SpriteValue spv;

    if (!mw2file.exists() || !mwtfile.exists())
        return 1;
    if (!mw2file.open(QIODevice::ReadOnly))
        return 1;
    if (!mwtfile.open(QIODevice::ReadOnly))
        return 1;
    mw2stream.setDevice(&mw2file);
    mwtstream.setDevice(&mwtfile);

    read_bytes = new char[SPRITE_MAX_DATA_SIZE];

    // Read single byte header.
    if (mw2stream.readRawData(read_bytes, 1) != 1 || *read_bytes != 0x00) {
        ret = 1;
        goto cleanup;
    }
    
    while (!mw2stream.atEnd()) {
        parseret = mw2_parse(mw2stream, spk, spv, read_bytes);
        if (parseret == 1) {
            ret = 1;
            goto cleanup;
        }
        if (parseret == 2)
            break;
        parseret = mwt_parse(mwtstream, mwt_id, mwt_name);
        if (parseret == 2 && mwt_id != spk.id) {
#ifdef DEBUG
            qDebug() << "ERROR: non matching IDs:" << spk.id << "and" << mwt_id << '\n';
#endif
            return 1;
        }
        spv.name = mwt_name;
        // Insert sprite. At this point, the information retrieved are:
        // ID, extra bits, name, extension bytes
#ifdef DEBUG
        qDebug() << "ID:" << spk.id << "extra bits:" << spk.extra_bits()
                 << "name:" << spv.name << '\n';
#endif
        sprite::sprite_insert(spk, spv);

    }
    
    // Check for errors with end of data byte.
    if (mw2stream.atEnd() && *read_bytes != (char) 0xFF) {
#ifdef DEBUG
        qDebug() << "Bad format";
#endif
        ret = 4;    // End of file byte error
    }

    if (!mwtstream.atEnd()) {
#ifdef DEBUG
        qDebug() << "The mwt file hasn't been read all the way. This might be a bug.";
#endif
    }

cleanup:    
    delete read_bytes;
    return ret;
    return 0;
}


/* Parses a single line and does some other stuff.  */
/*
int ssc_parse(QTextStream &mwtstream, sprite::SpriteKey &spk, sprite::SpriteValue &spv)
{
    QStringList strlist;
    bool ok;
    unsigned char id, extra_bits, line_type;

    strlist = mwtstream.readline().split('\t', QString::SkipEmptyParts);
    if (strlist.size() < 2)
        return 1;
    id = strlist[0].toInt(&ok, 16);
    extra_bits = strlist[1].toInt(&ok);
    if (!ok)
        return 1;
    line_type = extra_bits % 10;
    if (line_type != 0 && line_type != 2)
        return 1;
    extra_bits /= 10;
    if (extra_bits < 0 || extra_bits > 3)
        return 1;
    
    if (line_type == 0) {
        if (strlist.size() != 3)
            return 1;
        spv.tooltip = tooltip; // hopefully this just copies the string
        return 0;
    } else if (line_type == 2) {
               return 0;
    }
    return 1;
}*/

/* Reads .ssc file line by line. In an ssc file there can be two kind of 
 * lines:
 *  - (cmd)\t(type(20/30))\t(tooltip)\n
 *  - (cmd)\t(type(20/30))\t(spritetileinfo...)\n
 * On the first type, we just need to collect the tooltip.
 * On the second, we'll have to collect and insert every sprite tile.
 * Again, we have to get the command first to do any operation. */
/*
int ssc_readfile(const QString &romname)
{
   QFile sscfile(romname + ".ssc");
    QTextStream sscstream;
    QStringList strlist;
    int ncolumns, err, nlines = 0, extra_bits;
    bool ok;
 
    if (!sscfile.exists())
        return 2;
    if (!sscfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;
    
    sscstream.setDevice(&sscfile);
    while (!sscstream.atEnd()) {
        strlist = sscstream.readLine().split('\t',
                QString::SkipEmptyParts);
        nlines++;
        cmd = strlist[0].toInt(&ok, 16);
        if (!ok) {
            qDebug() << "Error at line" << nlines 
                << ": couldn't convert cmd";
            return 1;
        }
        extra_bits = strlist[1].toInt(&ok)/10;
        ncolumns = strlist.size();

        if (ncolumns < 3) {
            qDebug() << "Error at line" << nlines 
                << ": ncolumns less than 3";
            return 3;
        }
        // This is bad. Check what type of line it is can be done by
        // checking the extra bits column (column 1)
        if (ncolumns == 3 && strlist[2][0].isDigit()) // Tooltip
            get_sprite(cmd, extra_bits).tooltip = strlist[2];
        else {  // Sprite tiles
            strlist.removeAt(0);    // Remove cmd and extrabytes
            strlist.removeAt(1);
            err = get_sprite(cmd, extra_bits).parse_add_tile(strlist);
            if (err != 0) {
                qDebug() << "Error at line" << nlines 
                         << ": not a spritetile string";
            }
        }
    }
    sscfile.close();

    return 0;
}*/


}

