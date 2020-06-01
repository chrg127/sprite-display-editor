#include "sprite.h"

#include <QMultiMap>
#include <QDebug>

#define DEBUG

namespace sprite {

/* This is a lookup table for sprite sizes. It is effectively a copy of the 
 * real table in the ROM. It is worth noting that due to static initialization,
 * this array will always be initializated with 0s, instead of default value 3.
 * This is intentional, and can be used to check if sprite size have been
 * loaded or not. */
static unsigned char _size_table[SPRITE_SIZE_TABLE_MAX];

void load_size_table(smw::ROM &rom)
{
    unsigned int addr, pc_addr;
    int i;
    
    // Check if the table exists
    if (rom.data[smw::snestopc(0x0EF30F, rom.mapper)] != 0x42) {
        std::memset(_size_table, SPRITE_DEF_DATA_SIZE, SPRITE_SIZE_TABLE_MAX);
        return;
    }
    // Build table address and copy table
    addr = rom.data[smw::snestopc(0x0EF30E, rom.mapper)] << 16;
    addr |= rom.data[smw::snestopc(0x0EF30D, rom.mapper)] << 8;
    addr |= rom.data[smw::snestopc(0x0EF30C, rom.mapper)];
    pc_addr = smw::snestopc(addr, rom.mapper);
    i = 0;
    while (i != SPRITE_SIZE_TABLE_MAX) {
        _size_table[i] = rom.data[pc_addr];
#ifdef DEBUG
        if (rom.data[pc_addr] != 3)
            qDebug() << "At:" << addr << "size:" << rom.data[pc_addr] << "i:" << i;
#endif
        i++;
        pc_addr++;
    }
}

unsigned char SpriteKey::get_data_size(void)
{
    return _size_table[this->extra_bits()*0x100+this->id];
}



/* For using QMultiMap */
bool operator<(const SpriteKey &s1, const SpriteKey &s2)
{
    if (s1.id < s2.id)
        return true;
    if (s1.id == s2.id && s1.extra_bits() < s2.extra_bits())
        return true;
    return false;
}

bool operator==(const SpriteKey &sk1, const SpriteKey &sk2)
{
    return sk1.id == sk2.id && sk1.extra_bits() == sk2.extra_bits();
}

bool operator==(const SpriteTile &spt1, const SpriteTile &spt2)
{
    return spt1.x == spt2.x && spt1.y == spt2.y
        && spt1.map16tile == spt2.map16tile;
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




static QMultiMap<SpriteKey, SpriteValue> sprite_map;

int sprite_insert(SpriteKey &key, SpriteValue &value)
{
    if (sprite_map.contains(key, value))
        return 1;
    sprite_map.insert(key, value);
    return 0;
}

inline void remove_sprite(SpriteKey &key, SpriteValue &value)
{
    sprite_map.remove(key, value);
}

inline void remove_all_sprites(void)
{
    sprite_map.clear();
}

}
/*
int Sprite::parse_add_tile(const QStringList &tilelist)
{
    QStringList slist;
    SpriteTile st;
    int ret = 0;
    bool ok;

    for (auto &tilestr : tilelist) {
        slist = tilestr.split(',', 
            QString::SkipEmptyParts);
        if (slist.size() != 3) {
            qDebug() << "ERROR: Invalid size";
            ret++;
            continue;
        }
        st.x = slist[0].toInt(&ok, 0);
        st.y = slist[1].toInt(&ok, 0);
        st.map16tile = (unsigned short) slist[2].toInt(&ok, 16);
        tiles.append(st);
    }

    return ret;
}
*/

