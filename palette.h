/* 
 * palette.h: defines a struct for holding colors in the SNES format and
 * to collect palette tables from Super Mario World.
 */

#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

#include <cstdint>

namespace smw {
    struct ROM;
}

struct SNESColor {
    uint8_t blue;
    uint8_t green;
    uint8_t red;

    void fill(uint16_t bytes);
    uint16_t tobytes();
    int torgb();
    void fromrgb(int r, int g, int b);
}

enum ColorMasks : int {
    RED     = 0x001F,
    GREEN   = 0x03E0,
    BLUE    = 0x7C00,
}

// Sprite Palette x = $00B318 + (#$18 * x). 
// (Palette E,2 to E,7 and F,2 to F,7)
void get_global_sprite_palette(smw::ROM &rom, SNESColor *colortab);

#endif
