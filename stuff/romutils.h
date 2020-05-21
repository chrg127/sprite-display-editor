#ifndef _ROM_UTILS_H_
#define _ROM_UTILS_H_

namespace ROMUtils {

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

bool findmapper(mapper_t &rommapper, char *romdata);
int snestopc(int addr, mapper_t rommapper);
int pctosnes(int addr, mapper_t rommapper);

}

#endif

