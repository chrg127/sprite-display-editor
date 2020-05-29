/* sprite.h: not exactly a sprite class comprising every information about
 * sprites in super mario world. */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <QString>
#include <QVector>

class SpriteTile {
    public:
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


class Sprite {
    public:
        QString name;
        QString tooltip;
        unsigned char extra_bits;
        QVector<SpriteTile> tiles;
        unsigned char sprite_data_size; 
        unsigned char extension_bytes[0xF];
        
        Sprite()
        { 
            name = "";
            tooltip = "";
            extra_bits = 0;
        }

        Sprite(QString n, QString t, unsigned char eb)
        { 
            name = n;
            tooltip = t;
            extra_bits = eb;
        }

        Sprite(const Sprite &sp)
        {
            extra_bits = sp.extra_bits;
        }

        int parse_add_tile(const QStringList &s);

        void operator=(const Sprite &sp)
        {
            this->name = sp.name;
            this->tooltip = sp.tooltip;
            this->extra_bits = sp.extra_bits;
        }

};

#endif

