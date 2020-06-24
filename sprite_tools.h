#ifndef SPRITE_TOOLS_H
#define SPRITE_TOOLS_H

namespace smw {
    struct ROM;
}

/* Checks if sprites are inserted with PIXI.
 * If they are, then the 4 bytes at $02FFE2 are equal to "STSD".
 * (no, I have no idea what's the meaning of that string) */
bool check_pixi_inserted(smw::ROM &rom);

void find_pixi_sprites(smw::ROM &rom);

/* === Global Sprite Table ===
 * A table that holds bytes from the sprite's CFG file and its insertion address.
 * Example:
 * 01 36 0D 0C 30 BE 1A C4 B0 CA 10 C1 CA 10 00 00
 * And corresponding CFG file:
 * 01
 * 36
 * 0D 0C 30 BE 1A C4
 * 00 00
 * INIT address: $10CAB0
 * MAIN address: $10CAC1 */

#endif
