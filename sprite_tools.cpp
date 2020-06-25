#include <cstdio>
#include <cstring>
#include "ext/libsmw.h"

bool check_pixi_inserted(smw::ROM &rom)
{
    const unsigned char *pixistr = &(rom.data[smw::snestopc(0x02FFE2, &rom)]);
    if (std::strncmp((const char *) pixistr, "STSD", 4) != 0)
        return false;
    return true;
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

void find_pixi_sprites(smw::ROM &rom, int arrid[0xFF], unsigned int *sprnum)
{
    const int limit = 0x1000;
    unsigned int gltab_addr, initptr = 0, mainptr;
    unsigned char spriteinfo[0xF];
    int sprite_id;

    gltab_addr = rom.data[smw::snestopc(0x02FFF0, &rom)] << 16;
    gltab_addr |= rom.data[smw::snestopc(0x02FFEF, &rom)] << 8;
    gltab_addr |= rom.data[smw::snestopc(0x02FFEE, &rom)];
    gltab_addr = smw::snestopc(gltab_addr, &rom);

    for (int tab_offset = 0; tab_offset < limit; tab_offset += 16) {
        for (int i = 0; i < 16; i++)
            spriteinfo[i] = rom.data[gltab_addr+tab_offset+i];
#ifdef DEBUG
        print_sprite_info(spriteinfo);
#endif
        // Check if INIT and MAIN ptrs are valid.
        sprite_id = tab_offset/16;
        initptr = (spriteinfo[10] << 16);
        initptr |= (spriteinfo[9] << 8);
        initptr |= spriteinfo[8];
        if (initptr == RTL_PTR)
            continue;
        // save sprite in array.
        arrid[(*sprnum)++] = sprite_id;
    }
}

#ifdef DEBUG
print_sprite_info(char spriteinfo[0xF])
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

