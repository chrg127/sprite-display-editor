/* This is a library for reading and writing "Lunar Magic files", in particular the
 * mw2, mwt and ssc files. */

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

/*
 * Lunar Magic provides a way to use custom sprites without using the insert manual:
 * simply place the custom sprite in the first screen of a level and press Ctrl-Alt-F12.
 * When inserting a custom sprite this way, it will generate two files: an mw2 file,
 * containing binary sprite data and an mwt file, containing the name assigned to the
 * sprite. Additionally, two other files can be used: an ssc file, which will contain
 * sprite tooltips and an s16 file containing map16 data used by sprites.
 */

/* The mw2 file format is the same as the sprite data in a SMW level. The
 * first byte consists of sprite data header, which should always be 00.
 * Next is the list of all sprites. Each sprite is usually composed of 3 bytes:
 * yyyyEESY XXXXssss NNNNNNNN
 * yyyy - Y position
 * EE - Extra bits
 * XXXX - X position
 * ssss - Screen number
 * NNNNNNNN - Sprite ID (or command)
 * Last byte: end of data. Should be 0xFF.
 * The sprite data, however, can change depending on the size of the sprite. If the
 * sprite size is > 3, then any other byte after the first 3 are "extension bytes".
 * 
 * This function reads bytes from the stream and parses them. It will first read 3 bytes,
 * so that it can find the ID and extra bits. The other fields in the format are not needed
 * and are ignored.
 * After reading the first 3 bytes, it will read any remaining bytes according to the sprite
 * size: those are the sprite's extension bytes and will be saved in the sprite's SpriteValue.
 * */

/* This is how you get the other fields, anyway:
 * ypos = ((read_bytes[0] & 0xF0) >> 4) | ((read_bytes[0] & 1) << 4);
 * xpos = (read_bytes[1] & 0xF0) >> 4;
 * screennum = (read_bytes[1] & 0xF) | ((read_bytes[0] & 2) << 3); 
 */

/* Returns 1 for error, 2 for end of data byte. */
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

/* The mwt format is really simple: Each line is the name of the sprite (yes, including the sprite ID).
 * This function parses a single line from the mwt file. It essentially provides better error checking,
 * since LM does not check if the IDs written in the mwt file and what's in the mw2 file match (for example,
 * one could have a custom sprite with ID 0A and write that this is a sprite with ID 05). If one wants to check
 * if two sprites don't have matching IDs in the mw2 and mwt file, he may use this function.
 * It'll return 2 if it finds an ID and -1 if the stream is at end. */
static int mwt_parse(QTextStream &mwtstream, unsigned char &id, QString &name)
{
    QString first_word, rest_of_line = "";
    bool ok = false;
    
    if (mwtstream.atEnd())
        return -1;
    mwtstream >> first_word;
    id = first_word.toInt(&ok, 16);
    rest_of_line = mwtstream.readLine().trimmed();
    if (ok) {
        name = rest_of_line;
        return 1;
    }
    first_word += rest_of_line;
    name = first_word;
    return 0;
}

/* Simultaneously reads both the mw2 file and mwt file and returns the following error codes:
 *  - 0: Success
 *  - 1: The mw2 or mwt file does not exist (or can't open). In this case, it is best not to continue;
 *  - 2: The mw2 file has a bad format;
 *  - 3: The mwt file is longer than the mw2 file (I.E. has more lines and more possible information);
 *  - 4: The mw2 file is longer than the mwt file. In this case, all information found in the mw2 file should be kept;
 * Because reading the mwt file by itself would not really provide any information on with what to associated each
 * name, it is better to read sprite bytes and name simultaneously. This function also inserts sprites in the
 * global sprite structure; therefore, make sure to call load_size_table before calling this function, and, if
 * a fatal error is found, to clear the sprite structure.
 */
int mw2_mwt_readfile(const QString &romname)
{
    QFile mw2file(romname + ".mw2"), mwtfile(romname + ".mwt");
    QDataStream mw2stream;
    QTextStream mwtstream;
    char *read_bytes;
    int ret = 0, parseret;
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
        ret = 2;
        goto cleanup;
    }
    
    while (!mw2stream.atEnd()) {
        parseret = mw2_parse(mw2stream, spk, spv, read_bytes);
        if (parseret == 1) {
            ret = 2;
            goto cleanup;
        }
        // Check if mw2 end of file byte has been found -- the end of file byte has been read at this point
        if (parseret == 2)
            break;
        
        // It is worth mentioning that if the mwt stream is at end, but not the mw2 stream is not, the loop
        // will keep going and won't provide any name to new found sprites.
        if (!mwtstream.atEnd())
            spv.name = mwtstream.readLine().trimmed();
        else
            ret = 4;
        sprite::sprite_insert(spk, spv);
        spv.name = "";
    }
    
    // Check for errors with end of data byte.
    if (mw2stream.atEnd() && *read_bytes != (char) 0xFF) {
        ret = 2;
        goto cleanup;
    }

    if (!mwtstream.atEnd())
        ret = 3;

cleanup:
#ifdef DEBUG
    if (ret == 2)
        qDebug() << "ERROR: mw2_mwt_readfile: bad mw2 format";
    else if (ret == 3)
        qDebug() << "ERROR: mw2_mwt_readfile: the mwt file hasn't been read all the way.";
    else if (ret == 4)
        qDebug() << "ERROR: mw2_mwt_readfile: already finished reading mwt file. The mw2 file might contain extra entries.";
#endif
    mw2file.close();
    mwtfile.close();
    delete read_bytes;
    return ret;
}

/* The ssc file contains sprite tooltip and information about their graphics. Its format is:
 *  - (id)\t(type(20/30))\t(tooltip)\n
 *  - (id)\t(type(22/32))\t(spritetileinfo...)\n
 * The line must have at least 2 columns. The type column also indicates the line type: 
 * if the last digit is 0, then it contains a tooltip; if it's 2, then it contains sprite tiles.
 * The first digit is the extra bits, obviously.
 * Returns 1 for parsing error. */
int ssc_parse(QTextStream &sscstream)
{
    QStringList strlist;
    bool ok;
    int i, line_type;
    sprite::SpriteKey sk;
    QVector<sprite::SpriteValue *> svarr;
    //SpriteValue sv;
    
    // Read line and get ID and extra bits.
    strlist = sscstream.readLine().split('\t', QString::SkipEmptyParts);
    if (strlist.size() < 2)
        return 1;
    sk.id = strlist[0].toInt(&ok, 16);
    sk.extra_bits(strlist[1].toInt(&ok));
    if (!ok)
        return 1;
    // Find line type.
    line_type = sk.extra_bits() % 10;
    if (line_type != 0 && line_type != 2)
        return 1;
    sk.extra_bits(sk.extra_bits() / 10);

    // get sprite values on which to operate
    get_sprite_value(sk, svarr);
    if (svarr.size() == 0) {
        // deal with no sprite found
        return 1;
    }

    // type == 0: tooltip
    if (line_type == 0) {
        if (strlist.size() != 3)
            return 1;
        for (i = 0; i < svarr.size(); i++)
            svarr[i]->tooltip = strlist[2];
        return 0;
    }
    // type == 2: sprite tiles
    if (line_type == 2) {
        strlist.removeAt(0);
        strlist.removeAt(1);
        for (i = 0; i < strlist.size(); i++)
            qDebug() << strlist[i];
        return 0;
    }
    return 1;
}

/* Reads the ssc file. Returns 1 on error. */
int ssc_readfile(const QString &romname)
{
    QFile sscfile(romname + ".ssc");
    QTextStream sscstream;
    int parseret;
 
    if (!sscfile.exists())
        return 2;
    if (!sscfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;
    
    sscstream.setDevice(&sscfile);
    while (!sscstream.atEnd()) {
        parseret = ssc_parse(sscstream);
        if (parseret == 1) {
            qDebug() << "an error happened somewhere";
            return 1;
        }
    }
    sscfile.close();

    return 0;
}


}

