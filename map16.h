#ifndef MAP16_H
#define MAP16_H

#include <cstdint>

enum TileFields : int {
    XX = 0x40,
    YY = 0x80,
    PRIORITY = 0x20,
    PALETTE = 0x1C,
    TT = 0x3,
};

struct Maptile {
    struct {
        char offset;
        char x, y, priority, pal, tt;
    } tile8[4];

    void filltile8(const int which, const uint8_t b1, const uint8_t b2)
    {
        tile8[which].offset     = b1;
        tile8[which].y          = (b2 & YY)       >> 7;
        tile8[which].x          = (b2 & XX)       >> 6;
        tile8[which].priority   = (b2 & PRIORITY) >> 5;
        tile8[which].pal        = (b2 & PALETTE)  >> 2;
        tile8[which].tt         = (b2 & TT);
    }

    void writetile8(const int which, uint8_t *b1, uint8_t *b2)
    {
        *b1 = tile8[which].offset;
        *b2 = tile8[which].y         << 7;
        *b2 |= tile8[which].x        << 6;
        *b2 |= tile8[which].priority << 5;
        *b2 |= tile8[which].pal      << 2;
        *b2 |= tile8[which].tt;
    }
};

/* Default size in bytes for a 16x16 tile and a 8x8 tile. */
#define T16SIZE 8
#define T8SIZE 2

#endif
