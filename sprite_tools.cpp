#include "sprite_tools.h"

#include <cstdio>
#include <cstring>
#include "ext/libsmw.h"

namespace sprite {

/* If PIXI was used on a ROM, then the 4 bytes at $02FFE2 are equal to "STSD".
 * (no, I have no idea what's the meaning of that string) */
bool check_pixi_inserted(smw::ROM &rom)
{
    const unsigned char *pixistr = &(rom.data[smw::snestopc(0x02FFE2, &rom)]);
    if (std::strncmp((const char *) pixistr, "STSD", 4) != 0)
        return false;
    return true;
}

bool check_pixi_perlevel(smw::ROM &rom)
{
    int flags = rom.data[smw::snestopc(0x02FFE7, &rom)];
    return (flags & 0x1) == 1;
}

/*
 * PIXI uses various tables to place information about inserted sprites.
 * One of them is called the "global table". It holds information about
 * normal sprites and a pointer to it can be found at $02FFEE.
 * Example line:
 * 01 36 0D 0C 30 BE 1A C4 B0 CA 10 C1 CA 10 00 00
 * 01: Sprite type (0 = Tweak, 1 = Custom sprite, 3 = Generator or shooter)
 * 36: Acts like
 * 0D0C30BE1AC4: Tweaker bytes
 * 10CAB0: INIT pointer
 * 10C1CA: MAIN pointer
 * 00: Extra property byte 1
 * 00: Extra property byte 2
 */
#define RTL_BANK 0x01
#define RTL_HIGH 0x80
#define RTL_LOW 0x21
#define RTL_PTR 0x018021

void find_pixi_sprites(smw::ROM &rom, unsigned int arrid[SPRITE_ID_MAX], unsigned int *foundids)
{
    unsigned int gltab_addr, initptr, mainptr;
    unsigned char spriteinfo[TAB_LINE_LEN-1];
    int i, taboffs, tab_size;

    // Build pointer to table, then convert to PC.
    gltab_addr = rom.data[smw::snestopc(0x02FFF0, &rom)] << 16;
    gltab_addr |= rom.data[smw::snestopc(0x02FFEF, &rom)] << 8;
    gltab_addr |= rom.data[smw::snestopc(0x02FFEE, &rom)];
    gltab_addr = smw::snestopc(gltab_addr, &rom);

    // Loop through each line of the table. The line number corresponds to
    // the sprite ID.
    tab_size = 0x1000;
    *foundids = 0;
    for (taboffs = 0; taboffs < tab_size; taboffs += TAB_LINE_LEN) {
        for (i = 0; i < TAB_LINE_LEN; i++)
            spriteinfo[i] = rom.data[gltab_addr+taboffs+i];
#ifdef DEBUG
        print_sprite_info(spriteinfo);
#endif
        // Check if INIT and MAIN ptrs are valid.
        initptr = (spriteinfo[10] << 16);
        initptr |= (spriteinfo[9] << 8);
        initptr |= spriteinfo[8];
        if (initptr == RTL_PTR)
            continue;
        mainptr = (spriteinfo[13] << 16);
        mainptr |= (spriteinfo[12] << 8);
        mainptr |= spriteinfo[11];
        if (initptr == RTL_PTR)
            continue;
        // save sprite in array.
        arrid[(*foundids)++] = taboffs/TAB_LINE_LEN;
    }
}

#ifdef DEBUG
void print_sprite_info(spriteinfo[TAB_LINE_LEN-1])
{
    std::printf("Sprite %02X:\n", sprite_id);
    std::printf("Type: %02X\n", spriteinfo[0]);
    std::printf("Acts like: %02X\n", spriteinfo[1]);
    std::printf("Tweaker bytes: ");
    for (int j = 2; j <= 7; j++)
        std::printf("%02X ", spriteinfo[j]);
    std::printf("\n");
    std::printf("INIT ptr: %02X %02X %02X\n", spriteinfo[10], spriteinfo[9], spriteinfo[8]);
    std::printf("MAIN ptr: %02X %02X %02X\n", spriteinfo[13], spriteinfo[12], spriteinfo[11]);
    std::printf("Extra Property Byte 1: %02X\n", spriteinfo[14]);
    std::printf("Extra Property Byte 2: %02X\n", spriteinfo[15]);
    std::printf("\n");
}
#endif

} // namespace sprite

