#ifndef _TOOL_H_
#define _TOOL_H_

#include <QVector>
#include <QString>
#include <cstddef>
#include "sprite.h"

class Tool {
private:
    QVector<Sprite> spritelist;
    
    // Returns 1 if can't open, 2 if not existing
    void open_ssc_file(const QString &romname);
    void open_mwt_file(const QString &romname);
    void open_mw2_file(const QString &romname);

public:
    Tool()
    { }
    
    // Finds a sprite given the sprite's name.
    int find_sprite(const QString &s);
    // Swaps a sprite at index i by n positions.
    // A negative n moves the sprite to the left.
    void swap_sprite(size_t i, size_t n);
    
    int update_sprite(int i, Sprite &s);

    void get_sprite_list(const QString &romname);
};

#endif
