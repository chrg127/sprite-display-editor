#ifndef _TOOL_H_
#define _TOOL_H_

#include <QMap>
#include <QString>
#include "sprite.h"
#include "ext/libsmw.h"

// Might be changed in the future
typedef unsigned char SpriteID;

namespace romutils {

/* QMap provides fast lookup and keeps its elements sorted. This is
 * good, since we're going to do loads of lookups in the application,
 * and there's no reason to have elements unsorted,
 * Sprites are sorted by their Command, or ID. */
static QMap<SpriteID, Sprite> spritelist;

//void generate_sprite_list(const QString &romname);
int read_mw2_file(const QString &romname);
int read_mwt_file(const QString &romname);
int read_ssc_file(const QString &romname);
void check_sprite_extensions(smw::ROM &rom);

}
    
#endif
