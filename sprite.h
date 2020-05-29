/* sprite.h: not exactly a sprite class comprising every information about
 * sprites in super mario world. */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <iostream>
#include <QString>
#include <QVector>

struct SpriteTile {
    int x, y;
    unsigned short map16tile;

    SpriteTile()
    {
        x = y = 0;
        map16tile = 0;
    }

    SpriteTile(int xx, int yy, unsigned short tile)
    {
        x = xx;
        y = yy;
        map16tile = tile;
    }
};

/* Sprites can be somewhat complicated. Any two sprites may
 * have the ID, or command, but not the same extra bits; it
 * is also possible for a sprite to have same ID and extra
 * bits, but not the same extension bytes. This struct will
 * take all this information into account.
 * NOTE: it is worth nothing that sprites with the same ID
 * and extra bits must have the same size. This is not yet
 * taken into account, a better design must be thought of...
 */
#define SPRITE_MAX_DATA_SIZE 0xF
#define SPRITE_DEF_SIZE 0x3

/* Yes, this class is kind of a long boilerplate... */
class SpriteKey {
private:
    unsigned char extra_bits;
    unsigned char data_size;
public:
    unsigned char id;
    unsigned char ext_bytes[SPRITE_MAX_DATA_SIZE];
    
    SpriteKey()
    {
        id = extra_bits = 0;
        data_size = SPRITE_DEF_SIZE;
        ext_bytes[0] = 0;
        ext_bytes[1] = 0;
        ext_bytes[2] = 0;
    }
 
    unsigned char get_ext_size(void)
    {
        return data_size - SPRITE_DEF_SIZE;
    }

    unsigned char get_extra_bits(void) const
    {
        return extra_bits;
    }

    int set_extra_bits(unsigned char eb)
    {
        if (eb > 3) {
            std::cerr << "ERROR: invalid extra bits\n";
            extra_bits = 0;
            return 1;
        }
        extra_bits = eb;
    }

    unsigned char get_data_size(void) const
    {
        return data_size;
    }

    int set_data_size(unsigned char s)
    {
        if (s > SPRITE_MAX_DATA_SIZE || s < SPRITE_DEF_SIZE) {
            std::cerr << "ERROR: invalid data size\n";
            data_size = 0;
            return 1;
        }
        data_size = s;
    }



    bool operator==(SpriteKey &sk)
    {
        unsigned char i;

        if (id != sk.id)
            return false;
        if (extra_bits != sk.extra_bits)
            return false;
        if (data_size != sk.data_size)
            return false;
        for (i = 0; i < data_size-SPRITE_DEF_SIZE; i++)
            if (ext_bytes[i] != sk.ext_bytes[i])
                return false;
        return true;
    }

/*
    bool operator>(SpriteKey &sk)
    {
        return sk < *this;
    }

    bool operator<=(SpriteKey &sk)
    {
        if (*this == sk)
            return true;
        return *this < sk;
    }

    bool operator>=(SpriteKey &sk)
    {
        if (*this == sk)
            return true;
        return *this > sk;
    }*/
};

inline bool operator<(const SpriteKey &sk1, const SpriteKey &sk);

/* This struct instead contains generic sprite information that needn't
 * or shouldn't be used as part of SpriteKey, such as name and tooltip.*/
struct SpriteValue {
    QString name;
    QString tooltip;
    QVector<SpriteTile> tiles;
    
    SpriteValue()
    { 
        name = "";
        tooltip = "";
    }

    SpriteValue(QString n, QString t)
    { 
        name = n;
        tooltip = t;
    }

    SpriteValue(const SpriteValue &sp)
    {
        name = sp.name;
        tooltip = sp.tooltip;
    }

    //int parse_add_tile(const QStringList &s);

    void operator=(const SpriteValue &sp)
    {
        this->name = sp.name;
        this->tooltip = sp.tooltip;
    }
};
#endif

