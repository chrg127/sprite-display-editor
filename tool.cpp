#include "tool.h"

#ifndef DEBUG
#define DEBUG
#endif

#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include <QStringList>
#include <QtGlobal>
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"
#include "sprite.h"

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#endif

namespace romutils {

/*
 * This function reads bytes from the stream and parses them. It will first read 3 bytes,
 * so that it can find the ID and extra bits. The other fields in the format are not needed
 * and are ignored.
 * After reading the first 3 bytes, it will read any remaining bytes according to the sprite
 * size: those are the sprite's extension bytes and will be saved in the sprite's SpriteValue.
 *
 * This is how you get the other fields, for anyone insterested:
 * ypos = ((read_bytes[0] & 0xF0) >> 4) | ((read_bytes[0] & 1) << 4);
 * xpos = (read_bytes[1] & 0xF0) >> 4;
 * screennum = (read_bytes[1] & 0xF) | ((read_bytes[0] & 2) << 3); 
 *
 * Returns 1 for error, 2 for end of data byte. */
static int mw2_parse(QDataStream &mw2stream, sprite::SpriteKey &spk,
        sprite::SpriteValue &spv, char *read_bytes)
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

 /* This function parses a single line from the mwt file. It essentially provides better error checking,
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
        name = rest_of_line.trimmed();
        return 1;
    }
    first_word += rest_of_line.trimmed();
    name = first_word.trimmed();
    return 0;
}

int mw2_mwt_readfile(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        const QString &romname)
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

        if (!sprite_map.contains(spk, spv))
            sprite_map.insert(spk, spv);

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
    delete[] read_bytes;
    return ret;
}

/* Parses a single line from the ssc file and updates the sprite accordingly.
 * Returns 1 on error. */
static int ssc_parse(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        QTextStream &sscstream)
{
    QString word, ebword, tooltip;
    QStringList tilelist;
    int line_type, i, j;
    bool ok;
    sprite::SpriteKey sk;
    unsigned char eb;
    QVector<sprite::SpriteValue *> svarr;
    
    if (sscstream.atEnd())
        return 1;
    sscstream >> word >> ebword;
    sk.id = word.toInt(&ok, 16);
    eb = ebword.toInt(&ok);
    if (!ok)
        return 1;
    line_type = eb % 10;
    sk.extra_bits(eb/10);

    sprite::get_sprite_values(sprite_map, sk, svarr);

    if (line_type == 0) {
        tooltip = sscstream.readLine().trimmed();
        for (i = 0; i < svarr.size(); i++)
            svarr[i]->tooltip = tooltip;
        return 0;
    } else if (line_type == 2) {
        tilelist = sscstream.readLine().trimmed().split(' ', QString::SkipEmptyParts);
        for (i = 0; i < svarr.size(); i++) {
            for (j = 0; j < tilelist.size(); j++) {
                int err = svarr[i]->add_tile_str(tilelist[j]);
#ifdef DEBUG
                if (err == 1)
                    qDebug() << "Problem adding tile string. ID:" << sk.id << "extra bits:" << sk.extra_bits();
#endif
            }
        }
        return 0;
    }
    return 1;
}

int ssc_readfile(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        const QString &romname)
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
        parseret = ssc_parse(sprite_map, sscstream);
        if (parseret == 1) {
#ifdef DEBUG
            qDebug() << "ERROR: Bad format.";
#endif
            return 1;
        }
    }
    sscfile.close();

    return 0;
}

int mw2_writefile(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        const QString &outname)
{
    QFile mw2file(outname + ".mw2");
    QDataStream mw2stream;
    char buf[SPRITE_MAX_DATA_SIZE];
    unsigned int len, i;
    
    if (!mw2file.open(QIODevice::WriteOnly))
        return 1;
    mw2stream.setDevice(&mw2file);
    
    buf[0] = 0;
    len = 1;
    mw2stream.writeRawData(buf, len);
 
    for (auto it = sprite_map.begin(); it != sprite_map.end(); it++) {
        buf[0] = it.key().extra_bits() << 2;
        buf[1] = 0;
        buf[2] = it.key().id;
        for (i = 0; i < it.key().get_ext_size(); i++)
            buf[i+SPRITE_DEF_DATA_SIZE] = it.value().ext_bytes[i];
        len = it.key().get_data_size();
        buf[len] = 0;
        mw2stream.writeRawData(buf, len);
    }
 
    buf[0] = 0xFF;
    len = 1;
    mw2stream.writeRawData(buf, len);
    return 0;
}

int mwt_writefile(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        const QString &outname)
{
    QFile mwtfile(outname + ".mwt");
    QTextStream mwtstream;
    
    if (!mwtfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return 1;
    mwtstream.setDevice(&mwtfile);
    for (auto it = sprite_map.begin(); it != sprite_map.end(); it++) {
        if (it.value().name == "")
            continue;
        mwtstream << (it.value().name) << '\n';
    }
    mwtfile.close();

    return 0;
}

int ssc_writefile(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        const QString &outname)
{
    QFile sscfile(outname + ".ssc");
    QTextStream sscstream;
    int line_type;

    if (!sscfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return 1;
    sscstream.setDevice(&sscfile);
    
    line_type = 0;
    for (auto it = sprite_map.begin(); it != sprite_map.end(); it++) {
        if (it.value().tooltip == "")
            continue;
        sscstream << QString("%1").arg(it.key().id, 2, 16, QChar('0')).toUpper() << '\t';
        sscstream << (it.key().extra_bits()*10+line_type) << '\t';
        sscstream << it.value().tooltip << '\n';
    }
    line_type = 2;
    for (auto it = sprite_map.begin(); it != sprite_map.end(); it++) {
        if (it.value().tiles.size() == 0)
            continue;
        sscstream << QString("%1").arg(it.key().id, 2, 16, QChar('0')).toUpper() << '\t';
        sscstream << (it.key().extra_bits()*10 + line_type) << '\t';
        for (auto tile = it.value().tiles.begin(); tile != it.value().tiles.end(); tile++) {
            sscstream << tile->x << ',' << tile->y << ',';
            sscstream << hex << uppercasebase << uppercasedigits << tile->map16tile << ' ';
        }
        sscstream << '\n';
    }
        
    sscfile.close();
    return 0;
}

}

