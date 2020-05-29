#include "sprite.h"

#include <QString>
#include <QStringList>
#include <QDebug>

inline bool operator<(const SpriteKey &sk1, const SpriteKey &sk)
{
    unsigned char i;

    if (sk1.id < sk.id)
        return true;
    if (sk1.id == sk.id && sk1.get_extra_bits() < sk.get_extra_bits())
        return true;
    if (sk1.id == sk.id && sk1.get_extra_bits() == sk.get_extra_bits()
            && sk1.get_data_size() < sk.get_data_size())
        return true;
    if (sk1.id == sk.id && sk1.get_extra_bits() == sk.get_extra_bits()
            && sk1.get_data_size() == sk.get_data_size()) {
        for (i = 0; i < sk1.get_data_size() - SPRITE_DEF_SIZE; i++)
            if (sk1.ext_bytes[i] < sk.ext_bytes[i])
                return true;
    }
    return false;
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
        if (!ok) {
            ret++;
            continue;
        }
        st.y = slist[1].toInt(&ok, 0);
        if (!ok) {
            ret++;
            continue;
        }
        st.map16tile = (unsigned short) slist[2].toInt(&ok, 16);
        if (!ok) {
            ret++;
            continue;
        }
        tiles.append(st);
    }

    return ret;
}
*/
