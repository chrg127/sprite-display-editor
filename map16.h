#ifndef MAP16_H
#define MAP16_H

struct Maptile {
    struct {
        char offset;
        char x, y, priority, pal, tt;
    } tile8[4];
};

enum TileFields : int {
    X = 0x80,
    Y = 0x40,
    PRIORITY = 0x20,
    PALETTE = 0x1C,
    TT = 0x3,
};

#endif
