//#define DEBUG

#include "sprite.h"

#include <QString>
#include <QMultiMap>
#include <cstring>

#ifdef DEBUG
#include <QDebug>
#endif

namespace sprite {

/* This is a lookup table for sprite sizes. It is effectively a copy of the real table in the ROM.
 * It is worth noting that due to static initialization, this array will always be initializated
 * with 0s, instead of default value 3. This is intentional, and can be used to check if sprite size 
 * have been loaded or not. */

#define SIZE_TABLE_MAX 0x400
#define SIZE_TABLE_PEREB 0x100

static unsigned char _size_table[SIZE_TABLE_MAX];

void load_size_table(smw::ROM &rom)
{
    unsigned int addr, pc_addr;
    int i;

    // Check if the table exists
    if (rom.data[smw::snestopc(0x0EF30F, &rom)] != 0x42) {
        std::memset(_size_table, SPRITE_DEF_DATA_SIZE, SIZE_TABLE_MAX);
        return;
    }
    // Build table address and copy table
    addr = rom.data[smw::snestopc(0x0EF30E, &rom)] << 16;
    addr |= rom.data[smw::snestopc(0x0EF30D, &rom)] << 8;
    addr |= rom.data[smw::snestopc(0x0EF30C, &rom)];
    pc_addr = smw::snestopc(addr, &rom);
    i = 0;
    while (i != SIZE_TABLE_MAX)
        _size_table[i++] = rom.data[pc_addr++];
}



unsigned char SpriteKey::get_data_size(void) const
{
    return _size_table[this->extra_bits()*SIZE_TABLE_PEREB+this->id];
}

bool operator<(const SpriteKey &s1, const SpriteKey &s2)
{
    return s1.id < s2.id || (s1.id == s2.id && s1.extra_bits() < s2.extra_bits());
}

bool operator==(const SpriteKey &sk1, const SpriteKey &sk2)
{
    return sk1.id == sk2.id && sk1.extra_bits() == sk2.extra_bits();
}



bool operator==(const SpriteTile &st1, const SpriteTile &st2)
{
    return st1.x == st2.x && st1.y == st2.y && st1.map16tile == st2.map16tile;
}



int SpriteValue::string2tile(QString &tstr)
{
    SpriteTile st;
    QStringList tokens;
    bool ok;

    tokens = tstr.split(',');
    if (tokens.size() != 3)
        return 1;
    st.x = tokens[0].toInt(&ok);
    st.y = tokens[1].toInt(&ok);
    st.map16tile = tokens[2].toInt(&ok, 16);
    if (!ok)
        return 1;
    this->tiles.append(st);
    return 0;
}

bool operator==(const SpriteValue &sv1, const SpriteValue &sv2)
{
    if (sv1.name != sv2.name)
        return false;
    if (sv1.tooltip != sv2.tooltip)
        return false;
    if (sv1.tiles != sv2.tiles)
        return false;
    for (unsigned char i = 0; i < SPRITE_MAX_DATA_SIZE; i++)
        if (sv1.ext_bytes[i] != sv2.ext_bytes[i])
            return false;
    return true;
}



void get_values(SpriteMap &spmap, const SpriteKey &sk, QVector<SpriteValue *> &arr)
{
    int i = 0;
    // sv is initialized to default value, remember that
    sprite::SpriteValue sv;

    // Insert a new sprite value if none were found and return that
    auto it = spmap.find(sk);
    if (it == spmap.end()) {
        it = spmap.insert(sk, sv);
        arr.insert( 0, &(it.value() ));
        return;
    }
    while (it != spmap.end() && it.key() == sk) {
        arr.insert(i, &(it.value()));
        i++;
        it++;
    }
    return;
}

void get_value(SpriteMap &spmap, const SpriteKey &sk, 
        unsigned char ext_bytes[SPRITE_MAX_DATA_SIZE], SpriteValue *spv)
{
    SpriteValue sv;
    int i;

    auto it = spmap.find(sk);
    if (it == spmap.end()) {
        it = spmap.insert(sk, sv);
        spv = &it.value();
    }
    while (it != spmap.end() && it.key() == sk) {
        for (i = 0; i < SPRITE_MAX_DATA_SIZE; i++)
            if (it.value().ext_bytes[i] != ext_bytes[i])
                continue;
        spv = &it.value();
    }
}

#ifdef DEBUG
void print_sprites(const SpriteMap &spmap)
{
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
        qDebug() << "ID:" << it.key().id << "extra bits:" << it.key().extra_bits()
                 << "Name:" << it.value().name;
        qDebug() << "Tooltip:" << it.value().tooltip;
        qDebug() << "Extension bytes:";
        for (int i = 0; i < it.key().get_ext_size(); i++)
            qDebug() << it.value().ext_bytes[i];
        qDebug() << "Sprite tiles:";
        for (auto tile : it.value().tiles)
            qDebug() << "x:" << tile.x << "y:" << tile.y << "map16:" << tile.map16tile;
        qDebug() << "";
    }
}
#endif

}

