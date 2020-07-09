#include "map16.h"

enum TileFields : int {
    XX = 0x80,
    YY = 0x40,
    PRIORITY = 0x20,
    PALETTE = 0x1C,
    TT = 0x3,
};

void Maptile::filltile8(int which, uint16_t tbytes)
{
    tile8[which].offset     = tbytes & 0xFF00   >> 8;
    tile8[which].y          = tbytes & YY       >> 7;
    tile8[which].x          = tbytes & XX       >> 6;
    tile8[which].priority   = tbytes & PRIORITY >> 5;
    tile8[which].pal        = tbytes & PALETTE  >> 2;
    tile8[which].tt         = tbytes & TT       >> 3;
}

uint16_t Maptile::writetile8(int which)
{
    uint16_t tb;

    tb = tile8[which].offset    << 8;
    tb |= tile8[which].y        << 7;
    tb |= tile8[which].x        << 6;
    tb |= tile8[which].priority << 5;
    tb |= tile8[which].pal      << 2;
    tb |= tile8[which].tt       << 3;
}
