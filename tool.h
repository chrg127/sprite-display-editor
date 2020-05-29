#ifndef _TOOL_H_
#define _TOOL_H_

#include <QMap>
#include <QString>
#include "sprite.h"
#include "ext/libsmw.h"

// Might be changed in the future
typedef unsigned char SpriteID;

namespace romutils {

Sprite get_sprite(SpriteID id);
int find_sprite_size(const unsigned char id, const unsigned char type);
void check_sprite_extensions(smw::ROM &rom);
int mw2_readfile(const QString &romname);
int mwt_readfile(const QString &romname);
int ssc_readfile(const QString &romname);

}
    
#endif
