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

static QMultiMap<SpriteID, Sprite> spritelist;

/* These functions will provide ways to modify the sprite data structure */
Sprite get_sprite(SpriteID id, unsigned char eb)
{
    QList<Sprite> values;
    int i;
/*
    values = spritelist.values(id);
    for (i = 0; i < values.size(); i++)
        if (values.at(i).extra_bits == eb)
            return values.at(i);*/
    auto it = spritelist.find(id);

}

int remove_sprite(const SpriteID id, const unsigned char eb)
{

}

void remove_all_sprites(void)
{
    for (QMap<SpriteID, Sprite>::iterator it = spritelist.begin(); it != spritelist.end(); it++) {
        it = spritelist.erase(it);
    }
}

/* Extension bytes and custom sprite sizes are one of Lunar Magic's
 * features. It allows any sprite to have 4 additional bytes in the sprite
 * data and is used by sprite coders for additional features.
 * The number of extension bytes a sprite has is not, however, stored with
 * his data. Instead, since Lunar Magic can't modify the way the sprite
 * data is stored, it reads the byte at $0EF30F and checks if it's equal to
 * 0x42. If it is, then $0EF30C points to a table containing each sprite's
 * number of extension bytes. Example:
 * ... 30 D2 12 42 ...
 *     ^        ^
 *     address  byte check 
 * The table is divided in four tables of 0x100 bytes each: The first one 
 * corresponds to extra bit 0, the second to extra bit 1, etc. */

#define EXT_TABLE_MAX 0x400
#define EXT_TABLE_PERBIT 0x100
#define SPR_DEF_SIZE 3

/* This is where information about what sprites have extension bytes is
 * stored */
static struct {
    unsigned char id[EXT_TABLE_MAX];
    unsigned char size[EXT_TABLE_MAX];
    unsigned char type[EXT_TABLE_MAX];
    unsigned short eff_len;
} size_tab;

/* Gives the index into size_tab for a sprite of a given id and type.
 * Returns -1 if not found. */
int find_sprite_size(const unsigned char id, const unsigned char type)
{
    short i;

    for (i = 0; i < size_tab.eff_len; i++)
        if (size_tab.id[i] == id && size_tab.type[i] == type)
            return i;
    return -1;
}

/* Checks if the extension table exists and parses it, filling the structure 
 * defined above. */
void check_sprite_extensions(smw::ROM &rom)
{
    unsigned int addr, snes_addr;
    int i, type;
    
    // Check extensions existence byte
    size_tab.eff_len = 0;
    if (rom.data[smw::snestopc(0x0EF30F, rom.mapper)] != 0x42)
        return;
    // Build table address
    addr = rom.data[smw::snestopc(0x0EF30E, rom.mapper)] << 16;
    addr |= rom.data[smw::snestopc(0x0EF30D, rom.mapper)] << 8;
    addr |= rom.data[smw::snestopc(0x0EF30C, rom.mapper)];
    // Parse table. "type" indicates the current "extra bit".
    for (i = 0; i < EXT_TABLE_MAX; addr++, i++) {
        type = i/0x100;
        snes_addr = smw::snestopc(addr, rom.mapper);
        if (rom.data[snes_addr] == SPR_DEF_SIZE)
            continue;
        size_tab.id[size_tab.eff_len] = i & 0xFF;
        size_tab.size[size_tab.eff_len] = rom.data[snes_addr];
        size_tab.type[size_tab.eff_len] = type;
        size_tab.eff_len++;
#ifdef DEBUG
        qDebug() << i;
        qDebug() << "At:" << addr << "size:" << rom.data[smw::snestopc(addr, rom.mapper)]
                << "extrabyte:" << type << "id:" << (i & 0xFF);
#endif
    }
}


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

/* This is how you get the other fields, anyway: */
//ypos = ((read_bytes[0] & 0xF0) >> 4) | ((read_bytes[0] & 1) << 4);
//xpos = (read_bytes[1] & 0xF0) >> 4;
//screennum = (read_bytes[1] & 0xF) | ((read_bytes[0] & 2) << 3);

/* Reads the next sprite bytes from stream and fills sp.
 * Return 1 for error, 2 for end of data byte. */
int mw2_parsespritedata(QDataStream &mw2st, Sprite &sp,
        unsigned char &id, char *read_bytes)
{
    qint64 nread, toread;
    int i;
    
    if (mw2st.atEnd())
        return 0;
    // Read first 3 bytes and get the sprite's ID and extra bits.
    nread = mw2st.readRawData(read_bytes, SPR_DEF_SIZE);
    if (nread == 1 && *read_bytes == (char) 0xFF)
        return 2;
    if (nread != SPR_DEF_SIZE) {
#ifdef DEBUG
        qDebug() << "Error while reading first 3 sprite bytes.";
        qDebug() << "mw2stream state:" << mw2st.atEnd();
#endif
        return 1;
    }
    sp.extra_bits = (read_bytes[0] & 0xC) >> 2;
    id = read_bytes[2];

    // Read extension bytes if sprite size > 3.
    i = find_sprite_size(id, sp.extra_bits);
    if (i == -1)
        return 0;   // No extension bytes

    toread = size_tab.size[i] - SPR_DEF_SIZE;
    nread = mw2st.readRawData(read_bytes, toread);
    if (toread != nread) {
#ifdef DEBUG
        qDebug() << "Error while reading extension bytes.";
#endif
        return 1;
    }
    for (i = 0; i < nread; i++)
        sp.extension_bytes[i] = read_bytes[i];
    return 0;
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
int mw2_readfile(const QString &romname)
{
    QFile mw2file(romname + ".mw2");
    QDataStream mw2stream;
    char *read_bytes;
    int ret = 0, parseret;
    unsigned char id;
    Sprite sp;
 
    if (!mw2file.exists())
        return 1;   // File error
    if (!mw2file.open(QIODevice::ReadOnly))
        return 1;
    mw2stream.setDevice(&mw2file);
    read_bytes = new char[4];

    // Read single byte header.
    if (mw2stream.readRawData(read_bytes, 1) != 1 || *read_bytes != 0x00) {
        ret = 2;    // Byte header error
        goto cleanup;
    }
    
    // Read each sprite's bytes until end of data has been found.
    while (!mw2stream.atEnd()) {
        parseret = mw2_parsespritedata(mw2stream, sp, id, read_bytes);
        if (parseret == 2)
            break;
        if (parseret == 1) {
            ret = 3;
            goto cleanup;
        }
        //this is the magic line.
        spritelist.insert(id, sp);
    }
    
    // Check for errors with end of data byte.
    if (mw2stream.atEnd() && *read_bytes != (char) 0xFF) {
#ifdef DEBUG
        qDebug() << "Bad format";
#endif
        ret = 4;    // End of file byte error
    }

cleanup:    
    delete read_bytes;
    return ret;
}


/* Reads .mwt file line by line. Example line:
 * (cmd)\t(sprite name)\n
 * After splitting a line, it converts the command and finds the sprite 
 * using the command, then it will update its name. */
int read_mwt_file(const QString &romname)
{
    /*
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
        get_sprite(cmd, ).name = strlist[1];
    }
    mwtfile.close();
    return 0;*/
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
    int ncolumns, err, nlines = 0, extra_bits;
    bool ok;
    SpriteID cmd;
    
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
}


}

