// RudeGuy: asar's codebase is a fucking mess of global variables and includes. 
// Here is my attempt at "unglobalizing" every variable and have this entire thing
// make sense.

#pragma once

#ifndef _LIBSMW_H_
#define _LIBSMW_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "asar_errors_small.h"
//#include "autoarray.h"

namespace smw {

extern asar_errid openromerror;

//extern const unsigned char * romdata;
//extern int romlen;
//extern autoarray<writtenblockdata> writtenblocks;

enum mapper_t {
	invalid_mapper,
	lorom,
	hirom,
	sa1rom,
	bigsa1rom,
	sfxrom,
	exlorom,
	exhirom,
	norom
};

struct writtenblockdata {
	int pcoffset;
	int snesoffset;
	int numbytes;
};

struct ROM {
    FILE *file;
    const unsigned char *data;
    int lenght;
    mapper_t mapper;
    bool header;

    /*ROM()
    {
        file = nullptr;
        data = nullptr;
        lenght = 0;
        mapper = mapper_t::lorom;
        header = false;
    }*/
/*
    ~ROM()
    {
        if (file)
            fclose(file);
        if (data)
            free((void *)data);
    }*/
};

bool openrom(ROM *rom, const char * filename, bool confirm=true);
int closerom(ROM *rom, bool save = true);
uint32_t get_rom_crc(ROM *rom);

bool findmapper(ROM *rom);
int check_header(ROM *rom);

int snestopc(int addr, ROM *rom);
int pctosnes(int addr, ROM *rom);

inline void buildptr(unsigned int addr, unsigned char bank, 
        unsigned char page, unsigned char offset);

}
/*
void writeromdata(int pcoffset, const void * indata, int numbytes,
                const unsigned char * romdata, mapper_t rommapper);
                
void writeromdata_byte(int pcoffset, unsigned char indata,
                const unsigned char * romdata, mapper_t rommapper);
                
void writeromdata_bytes(int pcoffset, unsigned char indata, int numbytes,
                const unsigned char * romdata, mapper_t rommapper);
                


int getpcfreespace(mapper_t rommapper, const unsigned char *romdata, 
                    int size, bool isforcode, bool autoexpand=true, 
                    bool respectbankborders=true, bool align=false, 
                    unsigned char freespacebyte=0x00);

int getsnesfreespace(mapper_t rommapper, const unsigned char *romdata,
                    int size, bool isforcode, bool autoexpand=true, 
                    bool respectbankborders=true, bool align=false, 
                    unsigned char freespacebyte=0x00);

int ratsstart(int pcaddr, const unsigned char * romdata, mapper_t rommapper);
void resizerats(int snesaddr, int newlen, mapper_t rommapper);
void removerats(int snesaddr, unsigned char clean_byte, const unsigned char * romdata, mapper_t rommapper);



//This one calls func() for each RATS tag in the ROM. 
//The pointer is SNES format.
void WalkRatsTags(const unsigned char * romdata, mapper_t rommapper,
                void(*func)(int loc, int len, const unsigned char * romdata));

//This one calls func() for each metadata block in the RATS tag whose 
//contents (metadata) start at loc in the ROM. Do not replace name with 
//an invalid metadata name, and note that name is not null terminated.
void WalkMetadata(const unsigned char * romdata, mapper_t rommapper, int loc, 
            void(*func)(int loc, char * name, int len, const unsigned char * contents, const unsigned char *romdata))

bool goodchecksum(const unsigned char *romdata, mapper_t rommapper);

void fixchecksum(const unsigned char *romdata, mapper_t rommapper);
*/

#endif
