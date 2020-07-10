#include "palette.h"

#include <cstring>
#include <cstddef>
#include "ext/libsmw.h"

/*
 * 00B280: Palette 8 colours 2-5 in levels, the last four bytes (colours 6-7) are only available during the Nintendo Presents logo; after that, they are overwritten by the first two colours from Mario's palette.
 * $00B28C: Palette 9 colours 2-7 in levels.
 * $00B298: Ludwig palette, and palette A colours 2-7 in levels.
 * $00B2A4: Roy palette, and palette B colours 2-7 in levels.
 * $00B2B0: Palette C colours 2-7 in levels.
 * $00B2BC: Morton palette, and palette D colours 2-7 in levels
 *
 * Rows E-F: depends on number, starts at 00B318
 */

/* Gets the sprite palette and inserts it into paltab. */
void get_global_sprite_palette(smw::ROM &rom, SNESColor *paltab, uint8_t num)
{
    int offs = 0, i;
    uint16_t *block;

    // copy 4 colors only for row 8, then fill the others with 0
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B280); i < 4; i++, block++)
        paltab[offs++].fill(block[i]);
    paltab[offs++].fill(0);
    paltab[offs++].fill(0);
    paltab[offs++].fill(0);
    paltab[offs++].fill(0);
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B28C); i < 6; i++, block++)
        paltab[offs++].fill(block[i]);
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B298); i < 6; i++, block++)
        paltab[offs++].fill(block[i]);
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B2A4); i < 6; i++, block++)
        paltab[offs++].fill(block[i]);
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B2B0); i < 6; i++, block++)
        paltab[offs++].fill(block[i]);
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B2BC); i < 6; i++, block++)
        paltab[offs++].fill(block[i]);

    // copies both row E and row F
    for (i = 0, block = (uint16_t *) rom.block_at(0x00B2BC); i < 12; i++, block++)
        paltab[offs++].fill(block[i]);
}

