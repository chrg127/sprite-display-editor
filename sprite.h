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
        unsigned char command;
        unsigned char extrabytes;
        QVector<SpriteTile> tiles;
        QVector<unsigned char> extensions;
        
        Sprite()
        { 
            name = "";
            tooltip = "";
            command = 0;
            extrabytes = 0;
        }

        Sprite(QString n, QString t, unsigned char c, unsigned char eb)
        { 
            name = n;
            tooltip = t;
            command = c;
            extrabytes = eb;
        }

        int parse_add_tile(const QStringList &s);

        void operator=(const Sprite &s);

};

#endif

