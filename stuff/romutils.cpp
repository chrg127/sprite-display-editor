#include "romutils.h"

#include <cstdint>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace ROMUtils {

static int sa1banks[8] = { 0<<20, 1<<20, -1, -1, 2<<20, 3<<20, -1, -1 };

/*
 * SNES ROM headers are located at $00FFC0. They contain:
 * C0 - Internal ROM title (ASCII encoding)     - 21 bytes;
 * D5 - Mapping mode and high speed switch      - 1 byte:
 *   bit 7, 6: always set to 0;
 *   bit 5:    always set to 1;
 *   bit 4:    speed flag;
 *   bit 3-0:  mapping mode;
 * D6 - Cartridge type                          - 1 byte;
 *   bit 3-0:  chipset;
 *   bit 7-4:  co-processor;
 * D7 - ROM size (log2 (size in kB) rounded up) - 1 byte;
 * D8 - RAM size (log2 (size in kB) rounded up) - 1 byte;
 * D9 - Region                                  - 1 byte;
 * DA - Dev ID                                  - 1 byte;
 * DB - Version                                 - 1 byte;
 * DC - Checsum complement                      - 2 bytes;
 * DE - Checksum                                - 2 bytes;
 * E0 - Interrupt vectors                       - 32 bytes;
 *
 * If the 21st byte of the ROM title ($D4) is set to $00, then the header
 * uses a second expanded version. It uses bytes $B0-$BF and contains:
 * B0 - Unused, set to $00                      - 15 bytes;
 * BF - Third party chip                        - 1 byte;
 *    If the top 4 bits of offset $D6 are all set, then there's a custom
 *    third party chip and $BF identifies it.
 *
 * If the developer ID ($DA) is set to $33, then the header uses a third
 * version which frees the 21st byte of the ROM title and uses the other
 * unused bits at $B0. It contains:
 * B0 - Dev ID, ASCII encoded                   - 2 bytes;
 * B2 - Game code, ASCII encoded                - 4 bytes;
 * B6 - Unused, set to $00                      - 6 bytes;
 * BC - Flash size (log2 (size in kB) rounded up) - 1 byte;
 * BD - Expansion RAM, same formula             - 1 byte;
 * BE - Denotes a special version of the game   - 1 byte;
 * BF - Same as above.                          - 1 byte;
 * */

/* Find ROM's mapper. The mapping mode is stored in the lower 5 bits of $D5:
 *  - 0 0000: LoROM             - $20
 *  - 0 0001: HiROM             - $21
 *  - 0 0011: SA-1 ROM          - $23
 *  - 1 0000: LoROM + FastROM   - $30
 *  - 1 0001: HiROM + FastROM   - $31
 *  - 1 0010: ExLoROM           - $32
 *  - 1 0101: ExHiROM           - $35
 * The ROM header however will change position depending on the mapping 
 * mode. This means we have to try every mapping mode and check if the 
 * header found is the actual header. This is what the first for does.*/

#define ROM_TITLE_SIZE 21

bool findmapper(mapper_t &rommapper, char *romdata)
{
	int maxscore = -99999;
	mapper_t bestmap = mapper_t::lorom;
	mapper_t maps[] = { mapper_t::lorom, mapper_t::hirom, mapper_t::exlorom,
        mapper_t::exhirom };
    std::size_t mapid;

	for (mapid=0; mapid < sizeof(maps)/sizeof(maps[0]); mapid++)
	{
		rommapper = maps[mapid];
		int score = 0;
		int highbits = 0;
		bool foundnull = false;
		for (int i = 0; i < ROM_TITLE_SIZE; i++)
		{
	        unsigned char c = romdata[snestopc(0x00FFC0+i, rommapper)];
            // according to some documents, NUL terminated names are 
            // possible - but they shouldn't appear in the middle of 
            // the name
			if (foundnull && c)
                score-=4;
			if (c >= 128)
                highbits++;
			else if (std::isupper(c))
                score+=3;
			else if (c == ' ')
                score+=2;
			else if (std::isdigit(c))
                score+=1;
			else if (std::islower(c))
                score+=1;
			else if (c == '-')
                score+=1;
			else if (!c)
                foundnull = true;
			else
                score-=3;
		}

        //high bits set on some, but not all, bytes = unlikely to be a ROM
		if (highbits>0 && highbits<=14) 
            score-=21;

        //checksum doesn't match up to 0xFFFF? Not a ROM.
		if ((romdata[snestopc(0x00FFDE, rommapper)] ^ romdata[snestopc(0x00FFDC, rommapper)]) != 0xFF ||
			(romdata[snestopc(0x00FFDF, rommapper)] ^ romdata[snestopc(0x00FFDD, rommapper)]) != 0xFF) 
            score = -99999;

		if (score > maxscore) { //too lazy to check the real checksum
			maxscore = score;
			bestmap = rommapper;
		}
	}
	rommapper = bestmap;

	//detect oddball mappers
	int mapperbyte = romdata[snestopc(0x00FFD5, rommapper)];
	int romtypebyte = romdata[snestopc(0x00FFD6, rommapper)];
	if (rommapper == mapper_t::lorom)
		if (mapperbyte == 0x23 && (romtypebyte == 0x32 || romtypebyte == 0x34 || romtypebyte == 0x35)) 
            rommapper = mapper_t::sa1rom;

	return (maxscore>=0);
}

int snestopc(int addr, mapper_t rommapper)
{
    if (addr < 0 || addr > 0xFFFFFF)
        return -1; //not 24bit
    
    switch (rommapper) {
    case mapper_t::lorom:
        // randomdude999: The low pages ($0000-$7FFF) of banks 70-7D are used
        // for SRAM, the high pages are available for ROM data though
        if ((addr & 0xFE0000) == 0x7E0000 || //wram
            (addr & 0x408000) == 0x000000 || //hardware regs, ram mirrors, other strange junk
            (addr & 0x708000) == 0x700000) //sram (low parts of banks 70-7D)
            return -1;
        addr = ((addr & 0x7F0000) >> 1 | (addr & 0x7FFF));
        return addr;
    case mapper_t::hirom:
        if ((addr & 0xFE0000) == 0x7E0000 || //wram
            (addr & 0x408000) == 0x000000) //hardware regs, ram mirrors, other strange junk
            return -1;
        return addr & 0x3FFFFF;
    case mapper_t::exlorom:
        if ((addr & 0xF00000) == 0x700000 || //wram, sram
            (addr & 0x408000) == 0x000000) //area that shouldn't be used in lorom
            return -1;
        if (addr & 0x800000)
            addr = ((addr & 0x7F0000) >> 1 | (addr & 0x7FFF));
        else
            addr = ((addr & 0x7F0000) >> 1 | (addr & 0x7FFF)) + 0x400000;
        return addr;
    case mapper_t::exhirom:
        if ((addr & 0xFE0000) == 0x7E0000 || //wram
            (addr & 0x408000) == 0x000000) //hardware regs, ram mirrors, other strange junk
            return -1;
        if ((addr & 0xC00000) != 0xC00000)
            return (addr & 0x3FFFFF) | 0x400000;
        return addr & 0x3FFFFF;
    case mapper_t::sfxrom:
        // Asar emulates GSU1, because apparently emulators don't support the extra ROM data from GSU2
        if ((addr & 0x600000) == 0x600000 || //wram, sram, open bus
            (addr & 0x408000) == 0x000000 || //hardware regs, ram mirrors, rom mirrors, other strange junk
            (addr & 0x800000) == 0x800000) //fastrom isn't valid either in superfx
            return -1;
        if (addr & 0x400000)
            return addr & 0x3FFFFF;
        else
            return (addr & 0x7F0000) >> 1 | (addr & 0x7FFF);
    case mapper_t::sa1rom:
        if ((addr & 0x408000) == 0x008000)
            return sa1banks[(addr & 0xE00000) >> 21] | ((addr & 0x1F0000) >> 1) | (addr & 0x007FFF);
        if ((addr & 0xC00000) == 0xC00000)
            return sa1banks[((addr & 0x100000) >> 20) | ((addr & 0x200000) >> 19)] | (addr & 0x0FFFFF);
        return -1;
    case mapper_t::bigsa1rom:
        if ((addr & 0xC00000) == 0xC00000) //hirom
            return (addr & 0x3FFFFF) | 0x400000;
        if ((addr & 0xC00000) == 0x000000 || (addr & 0xC00000) == 0x800000) //lorom
            if ((addr & 0x008000) == 0x000000)
                return -1;
            return (addr & 0x800000) >> 2 | (addr & 0x3F0000) >> 1 | (addr & 0x7FFF);
        return -1;
    case mapper_t::norom:
        return addr;
    }
    return -1;
}

int pctosnes(int addr, mapper_t rommapper)
{
    if (addr < 0)
        return -1;

    switch (rommapper) {
    case mapper_t::lorom:
        if (addr >= 0x400000)
            return -1;
        addr = ((addr << 1) & 0x7F0000) | (addr & 0x7FFF) | 0x8000;
        return addr | 0x800000;
    case mapper_t::hirom:
        if (addr >= 0x400000)
            return -1;
        return addr | 0xC00000;
    case mapper_t::exlorom:
        if (addr >= 0x800000)
            return -1;
        if (addr & 0x400000) {
            addr -= 0x400000;
            addr = ((addr << 1) & 0x7F0000) | (addr & 0x7FFF) | 0x8000;
            return addr;
        } else {
            addr = ((addr << 1) & 0x7F0000) | (addr & 0x7FFF) | 0x8000;
            return addr | 0x800000;
        }
    case mapper_t::exhirom:
        if (addr >= 0x800000)
            return -1;
        if (addr & 0x400000)
            return addr;
        return addr | 0xC00000;
    case mapper_t::sa1rom:
        for (int i = 0; i < 8; i++) {
            if (sa1banks[i] == (addr & 0x700000))
                return 0x008000 | (i << 21) | ((addr & 0x0F8000) << 1) | (addr & 0x7FFF);
        }
        return -1;
    case mapper_t::bigsa1rom:
        if (addr >= 0x800000)
            return -1;
        if ((addr & 0x400000) == 0x400000)
            return addr | 0xC00000;
        if ((addr & 0x600000) == 0x000000)
            return ((addr << 1) & 0x3F0000) | 0x8000 | (addr & 0x7FFF);
        if ((addr & 0x600000) == 0x200000)
            return 0x800000 | ((addr << 1) & 0x3F0000) | 0x8000 | (addr & 0x7FFF);
        return -1;
    case mapper_t::sfxrom:
        if (addr >= 0x200000)
            return -1;
        return ((addr << 1) & 0x7F0000) | (addr & 0x7FFF) | 0x8000;
    case mapper_t::norom:
        return addr;
    }
    return -1;
}

static FILE *thisfile;
static bool header;
static const unsigned char *romdata = nullptr;
static int romlen;
static mapper_t mapper = mapper_t::lorom;

#define SMWTITLE "SUPER MARIOWORLD     "

bool openrom(const char * filename, bool confirm)
{
	thisfile = std::fopen(filename, "r+b");
	if (!thisfile) {
		//openromerror = error_id_open_rom_failed;
		return false;
	}

    std::fseek(thisfile, 0, SEEK_END);
	header = false;
	if (std::strlen(filename) > 4) {
		const char * fnameend = strchr(filename, '\0')-4;
		header = (!std::stricmp(fnameend, ".smc"));
	}

	romlen = std::ftell(thisfile)-(header*512);
    romlen = (romlen < 0) ? 0 : romlen;

    std::fseek(thisfile, header*512, SEEK_SET);
	romdata = (const unsigned char *) std::malloc(sizeof(unsigned char)*16*1024*1024);

	int truelen = (int) std::fread(const_cast<unsigned char*>(romdata), 1u, (size_t) romlen, thisfile);
	if (truelen != romlen) {
		//openromerror = error_id_open_rom_failed;
        std::free(const_cast<unsigned char*>(romdata));
		return false;
	}

	memset(const_cast<unsigned char*>(romdata)+romlen, 0x00, (size_t)(16*1024*1024-romlen));
	if (confirm && snestopc(0x00FFC0, mapper)+21<(int)romlen 
            && strncmp((const char*)romdata + snestopc(0x00FFC0, mapper), SMWTITLE, ROM_TITLE_SIZE)) {
		closerom(false);
		//openromerror = header ? error_id_open_rom_not_smw_extension : error_id_open_rom_not_smw_header;
		return false;
	}
	return true;
}

}
