//#define DEBUG

#include "file_formats.h"

#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QStringList>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <cassert>
#include "ext/libsmw.h"
#include "sprite_defines.h"
#include "map16.h"

#ifdef DEBUG
#include <QDebug>
#endif

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

/* Parses a single line from the .mwt file and does format error checking.
 * Returns 1 if the stream is at end, 2 for format error. (not used in the program) */
/*static int mwt_parse(QTextStream &mwtstream, unsigned char &id, QString &name)
{
    QString first_word, rest_of_line = "";
    bool ok = false;
    
    if (mwtstream.atEnd())
        return 1;
    mwtstream >> first_word;
    id = first_word.toInt(&ok, 16);
    rest_of_line = mwtstream.readLine().trimmed();
    if (ok) {
        name = rest_of_line.trimmed();
        return 2;
    }
    first_word += rest_of_line.trimmed();
    name = first_word.trimmed();
    return 0;
}*/

/* Parses a single line from the ssc file and updates the sprite accordingly.
 * Returns 1 on error, 2 if stream is at end. */
static int ssc_parse(QTextStream &sscstream, sprite::SpriteMap &spmap)
{
    QString word, ebword, tooltip;
    QStringList tilelist;
    int line_type, i, j;
    bool ok;
    sprite::SpriteKey sk;
    unsigned char eb;
    QVector<sprite::SpriteValue *> svarr;

    if (sscstream.atEnd())
        return 2;
    sscstream >> word >> ebword;
    sk.id = word.toInt(&ok, 16);
    eb = ebword.toInt(&ok);
    if (!ok)
        return 1;
    line_type = eb % 10;
    sk.extra_bits(eb/10);

    if (sprite::get_values(spmap, sk, svarr) == 1) {
        sprite::SpriteValue sv;
        auto it = spmap.insert(sk, sv);
        svarr.append( &it.value() );
    }

    switch (line_type) {
    case 0:
        tooltip = sscstream.readLine().trimmed();
        for (i = 0; i < svarr.size(); i++)
            svarr[i]->tooltip = tooltip;
        return 0;
    case 2:
        tilelist = sscstream.readLine().trimmed().split(' ', QString::SkipEmptyParts);
        for (i = 0; i < svarr.size(); i++)
            for (j = 0; j < tilelist.size(); j++)
                if (svarr[i]->str2tile(tilelist[j]) == 1)
                    return 1;
        return 0;
    }
    return 1;
}



int mw2_mwt_readfile(sprite::SpriteMap &spmap, const QString &romname)
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

        if (!spmap.contains(spk, spv))
            spmap.insert(spk, spv);

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

int ssc_readfile(sprite::SpriteMap &spmap, const QString &romname)
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
        parseret = ssc_parse(sscstream, spmap);
        if (parseret == 1) {
#ifdef DEBUG
            qDebug() << "ssc_readfile: bad format";
#endif
            return 1;
        } else if (parseret == 2) {
#ifdef DEBUG
            qDebug() << "ssc_readfile: already at end";
#endif
        }

    }
    sscfile.close();

    return 0;
}

int mw2_writefile(sprite::SpriteMap &spmap, const QString &outname)
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

    for (auto it = spmap.begin(); it != spmap.end(); it++) {
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

int mwt_writefile(sprite::SpriteMap &spmap, const QString &outname)
{
    QFile mwtfile(outname + ".mwt");
    QTextStream mwtstream;

    if (!mwtfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return 1;
    mwtstream.setDevice(&mwtfile);
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
        if (it.value().name == "")
            continue;
        mwtstream << (it.value().name) << '\n';
    }
    mwtfile.close();

    return 0;
}

int ssc_writefile(sprite::SpriteMap &spmap, const QString &outname)
{
    QFile sscfile(outname + ".ssc");
    QTextStream sscstream;
    int line_type;

    if (!sscfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return 1;
    sscstream.setDevice(&sscfile);
    line_type = 0;
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
        if (it.value().tooltip == "")
            continue;
        sscstream << QString("%1").arg(it.key().id, 2, 16, QChar('0')).toUpper() << '\t';
        sscstream << (it.key().extra_bits()*10+line_type) << '\t';
        sscstream << it.value().tooltip << '\n';
    }
    line_type = 2;
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
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

int s16_readfile(Maptile arrtiles[0x2000], char *romname)
{
    std::FILE *sfile;
    unsigned char buf[16];
    std::size_t bread;
    int tile16no, tile8no;

    sfile = std::fopen(std::strcat(romname, ".s16"), "rb");
    if (!sfile)
        return 1;

    tile16no = tile8no = 0;
    while (bread = std::fread(buf, 1, 2, sfile), bread != 0) {
        if (bread != 2 && bread != 0)
            return 2;
        arrtiles[tile16no].tile8[tile8no].offset = buf[0];
        arrtiles[tile16no].tile8[tile8no].y = buf[1] & TileFields::Y >> 7;
        arrtiles[tile16no].tile8[tile8no].x = buf[1] & TileFields::X >> 6;
        arrtiles[tile16no].tile8[tile8no].priority = buf[1] & TileFields::PRIORITY >> 5;
        arrtiles[tile16no].tile8[tile8no].pal = buf[1] & TileFields::PALETTE >> 2;
        arrtiles[tile16no].tile8[tile8no].tt = buf[1] & TileFields::TT >> 3;
        if (++tile8no == 4) {
            tile8no = 0;
            tile16no++;
        }
    }
    if (tile16no != 0x2000)
        return 2;

    return 0;
}

int s16_writefile(Maptile arrtiles[0x2000], char *romname)
{
    std::FILE *sfile;
    int tile16no, tile8no;
    std::size_t bwritten;
    char buf[16];

    sfile = std::fopen(std::strcat(romname, ".s16"), "w");
    if (!sfile)
        return 1;

    for (tile16no = 0; tile16no < 0x2000; tile16no++) {
        for (tile8no = 0; tile8no < 4; tile8no++) {
            buf[0] = arrtiles[tile16no].tile8[tile8no].offset;
            buf[1] = arrtiles[tile16no].tile8[tile8no].y << 7;
            buf[1] |= arrtiles[tile16no].tile8[tile8no].x << 6;
            buf[1] |= arrtiles[tile16no].tile8[tile8no].priority << 5;
            buf[1] |= arrtiles[tile16no].tile8[tile8no].pal << 2;
            buf[1] |= arrtiles[tile16no].tile8[tile8no].tt << 3;
            bwritten = fwrite(buf, 1, 2, sfile);
            if (bwritten != 2)
                return 1;
        }
    }

    return 0;
}

