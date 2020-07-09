#ifndef MAP16_H
#define MAP16_H

#include <cstdint>

struct Maptile {
    struct {
        char offset;
        char x, y, priority, pal, tt;
    } tile8[4];

    void filltile8(int which, uint16_t tbytes);
    uint16_t writetile8(int which);
};

#endif
