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

void find_pixi_sprites(smw::ROM &rom)
{
    const int limit = 0x1000;
    unsigned int gltab_addr;

    gltab_addr = rom.data[smw::snestopc(0x02FFF0, &rom)] << 16;
    gltab_addr |= rom.data[smw::snestopc(0x02FFEF, &rom)] << 8;
    gltab_addr |= rom.data[smw::snestopc(0x02FFEE, &rom)];
    gltab_addr = smw::snestopc(gltab_addr, &rom);

    for (int tab_offset = 11; tab_offset < limit; tab_offset += 10) {
        for (int i = 0; i < 10; i++) {
            std::printf("%2hhX ", rom.data[gltab_addr+tab_offset+i]);
        }
        std::printf("\n");
    }
}
