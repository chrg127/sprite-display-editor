#pragma once

#include "errors.h"
#include "autoarray.h"
#include <cstdint>

extern const unsigned char * romdata;
extern int romlen;
extern asar_error_id openromerror;
extern autoarray<writtenblockdata> writtenblocks;

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

void writeromdata(int pcoffset, const void * indata, int numbytes, const unsigned char * romdata);
void writeromdata_byte(int pcoffset, unsigned char indata, const unsigned char * romdata);
void writeromdata_bytes(int pcoffset, unsigned char indata, int numbytes, const unsigned char * romdata);
bool openrom(const char * filename, bool confirm=true);
uint32_t closerom(bool save = true);

int snestopc(int addr);
int pctosnes(int addr);

int getpcfreespace(int size, bool isforcode, bool autoexpand=true, 
                    bool respectbankborders=true, bool align=false, 
                    unsigned char freespacebyte=0x00, mapper_t rommapper);
int getsnesfreespace(int size, bool isforcode, bool autoexpand=true, 
                    bool respectbankborders=true, bool align=false, 
                    unsigned char freespacebyte=0x00);

void resizerats(int snesaddr, int newlen);
void removerats(int snesaddr, unsigned char clean_byte, const unsigned char * romdata);
int ratsstart(int pcaddr, const unsigned char * romdata);

bool goodchecksum();
void fixchecksum();

//This one calls func() for each RATS tag in the ROM. 
//The pointer is SNES format.
void WalkRatsTags(void(*func)(int loc, int len), const unsigned char * romdata);
//This one calls func() for each metadata block in the RATS tag whose 
//contents (metadata) start at loc in the ROM. Do not replace name with 
//an invalid metadata name, and note that name is not null terminated.
void WalkMetadata(int loc, void(*func)(int loc, char * name, int len, const unsigned char * contents));

