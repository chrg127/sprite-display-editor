/* 
 * sprite.h: a library comprising of classes and structs for representing sprites in Super Mario World.
 *
 * Sprites contain a big number of attributes: ID, extra bits, name, graphics,
 * etc... In this library, I've decided to limit them to what is useful in
 * this program, however it might be extended for other uses.
 *
 * It is difficult to denote the _essential_ information to address a sprite,
 * and to denote what constitutes non-essential information, such as the name.
 * In general, these are the rules:
 *  - A sprite may have an associated ID and type. The type is often called
 *    "extra bits".
 *  - The ID is a number between 00 and FF.
 *  - The type is a number that can be only 0, 1, 2 or 3. Lunar Magic interprets
 *    numbers higher than 3 to just overflow back to these values.
 *  - ID and extra bits together are called "Custom Sprite Command" by Lunar
 *    Magic.
 *  - A sprite has an internal representation in sprite data, which stores,
 *    other than ID and extra bits, information about where is it placed.
 *    This representation is usually 3 bytes long, but sizes can vary. The
 *    actual data size of a sprite is in a table somewhere in the ROM (more
 *    information below on its respective functions). It is worth noting that
 *    these variable sizes are a third party feature, not used in the original
 *    game.
 *  - The custom sizes are usually there to enable a Lunar Magic feature:
 *    extension bytes. They aren't used very much, but must still be taken into
 *    account. Practically, this means that 2 sprites with the same ID and extra
 *    bits AREN'T the same sprite if one extension byte exists. This can get to
 *    weird territory... For example, Lunar Magic won't let you set up a
 *    custom tooltip for sprites with just different extension bytes, even
 *    if the feature is technically allowed.
 *  - Despite this, sprite with the same ID and extra bits must have the same
 *    size.
 * The design in this library attempts to solve all these problems efficiently.
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <QString>
#include <QVector>
#include <QDebug>
#include <cstring>
#include "ext/libsmw.h"

namespace sprite {

/* Let's talk about sprite sizes first.
 * The table may or may not exist, as it is the case with an unchanged SMW
 * ROM. If is exists, then $0EF30F should be 42, and $0EF30C points to a table 
 * containing each sprite's number of extension bytes. Example:
 * ... 30 D2 12 42 ...
 *     ^        ^
 *     address  byte check 
 * The table is divided in four tables of 0x100 bytes each: The first one 
 * corresponds to extra bit 0, the second to extra bit 1, etc. */

#define SPRITE_MAX_DATA_SIZE 0xF
#define SPRITE_DEF_DATA_SIZE 0x3
#define SPRITE_SIZE_TABLE_MAX 0x400

/* Structs and classes */

/* As the name clearly implies, use this class as the key of some data structure
 * for containing sprites. 
 * It provides encapsulation for extra_bits, so we can use the same system as 
 * Lunar Magic for changing the extra bits (if > 3, roll over). It doesn't do
 * so for id, because there's no reason for it to have special getters and
 * setters. */
class SpriteKey {
private:
    unsigned char _extra_bits;
public:
    unsigned char id;
    
    SpriteKey()
        : _extra_bits(0), id(0)
    { }
    
    // Set bit 2-7 of extra_bits to 0
    SpriteKey(unsigned char m_id, unsigned char m_extra_bits)
        : _extra_bits(m_extra_bits & 3), id(m_id)
    { }

    ~SpriteKey()
    { }

    unsigned char extra_bits(void) const 
    {
        return _extra_bits;
    }

    void extra_bits(unsigned char eb)
    {
        _extra_bits = eb & 3;
    }
    
    // Could easily be inline... but this way I avoid having an extern declaration of size_table
    unsigned char get_data_size(void) const;

    unsigned char get_ext_size(void) const
    {
        return get_data_size() - SPRITE_DEF_DATA_SIZE;
    }
};


/* Sprite Tile data, contained in the mwt file. */
struct SpriteTile {
    int x, y;
    unsigned short map16tile;

    SpriteTile()
        : x(0), y(0), map16tile(0)
    { }

    SpriteTile(int xx, int yy, unsigned short tile)
        : x(xx), y(yy), map16tile(tile)
    { }

    ~SpriteTile()
    { }
};

/* Other sprite data that's not important enough to put into SpriteKey. */
struct SpriteValue {
    QString name;
    QString tooltip;
    QVector<SpriteTile> tiles;
    unsigned char ext_bytes[SPRITE_MAX_DATA_SIZE];
    
    SpriteValue()
        : name(""), tooltip(""), tiles()
    {
        for (unsigned char i = 0; i < SPRITE_MAX_DATA_SIZE; i++)
            ext_bytes[i] = 0;
    }

    ~SpriteValue()
    { }

    int add_tile_str(QString &tstr);
};



/* Operator functions */
bool operator<(const SpriteKey &sk1, const SpriteKey &sk2);
bool operator==(const SpriteKey &sk1, const SpriteKey &sk2);
bool operator==(const SpriteTile &st1, const SpriteTile &st2);
bool operator==(const SpriteValue &sv1, const SpriteValue &sv2);



/* Function prototypes */

/* Copies the size table from the ROM.
 * If the table doesn't exist, it fills the local size table with the default size. */
void load_size_table(smw::ROM &rom);

/* These functions are an interface to a user-implemented sprite container.
 * The sprite container in this case is a QMultiMap, but it could have been
 * anything that lets us reference a sprite using just its ID and extra bits.
 * Of note, QMultiMap was chosen so it allows us to store sprites with the
 * same ID and value, but with different extension bytes. */

/* Gets a reference to each sprite value for a SpriteKey. If no sprite values were
 * found, it'll insert a new one for that key, then get a reference to it.
 * All the references are put in a vector. */
void get_sprite_values(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map,
        const SpriteKey &sk, QVector<SpriteValue *> &arr);
void print_sprites(QMultiMap<sprite::SpriteKey, sprite::SpriteValue> &sprite_map);

}


#endif

