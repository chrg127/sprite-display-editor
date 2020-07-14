/* 
 * palette.h: defines a struct for holding colors in the SNES format and
 * to collect palette tables from Super Mario World.
 */

#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

#include <cstdint>
#include <cstdio>

namespace smw {
    struct ROM;
}

enum ColorMasks : int {
    RED     = 0x001F,
    GREEN   = 0x03E0,
    BLUE    = 0x7C00,
};

struct SNESColor {
    uint8_t blue;
    uint8_t green;
    uint8_t red;

    void fill(uint16_t bytes)
    {
        red     = bytes & RED;
        green   = bytes & GREEN;
        blue    = bytes & BLUE;
    }

    uint16_t tobytes()
    {
        uint16_t toret = red;
        toret |= green << 5;
        toret |= blue << 10;
        return toret;
    }

    void torgb(int *r, int *g, int *b)
    {
        *r = red*8;
        *g = green*8;
        *b = blue*8;
    }

    void fromrgb(int r, int g, int b)
    {
        red = r/8;
        green = g/8;
        blue = b/8;
    }

    void print()
    {
        std::printf("red = %02X, green = %02X, blue = %02X\n", red, green, blue);
    }
};

/* Gets the sprite palette (row 8-F) from a SMW ROM. */
void get_global_sprite_palette(smw::ROM &rom, SNESColor *paltab, uint8_t num);

#endif
