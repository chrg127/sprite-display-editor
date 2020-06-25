#ifndef SPRITE_TOOLS_H
#define SPRITE_TOOLS_H

#include "sprite_defines.h"

namespace smw {
    struct ROM;
}

#define TAB_LINE_LEN 16

namespace sprite {

/* Checks if sprites are inserted with PIXI. */
bool check_pixi_inserted(smw::ROM &rom);

/* Checks if PIXI's per-level feature is enabled. */
bool check_pixi_perlevel(smw::ROM &rom);

/* Finds the IDs of all inserted normal sprites (I.E. not shooters, generators, etc.)
 * and puts them inside arrid, with sprnum indicating the number of sprites found.
 * This function assumes that no PIXI features, such as per-level sprites were used. */
void find_pixi_sprites(smw::ROM &rom, unsigned int arrid[SPRITE_ID_MAX], unsigned int *sprnum);

/* Prints sprite info from the global table. Use it after reading a line from it. */
#ifdef DEBUG
void print_sprite_info(unsigned int spriteinfo[TAB_LINE_LEN-1]);
#endif

} // namespace sprite

#endif
