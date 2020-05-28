#include "sprite.h"

#include <QString>
#include <QStringList>
#include <QDebug>

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

