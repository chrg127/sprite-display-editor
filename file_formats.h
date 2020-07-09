/* This is a library for reading and writing "Lunar Magic files", in particular the
 * mw2, mwt and ssc files. */

/*
 * Lunar Magic provides a way to use custom sprites without using the insert manual:
 * simply place the custom sprite in the first screen of a level and press Ctrl-Alt-F12.
 * When inserting a custom sprite this way, it will generate two files:
 *  - A .mw2 file, containing binary sprite data;
 *  - A .mwt file, containing the name assigned to the sprite.
 * Additionally, two other files can be used: 
 *  - A .ssc file, which will contain sprite tooltips
 *  - A .s16 file containing map16 data used by sprites.
 */

/* 
 * I'll describe each file's format here.
 * 
 * The mw2 file format is the same as the sprite data in a SMW level. The
 * first byte consists of sprite data header, which should always be 00.
 * Next is the list of all sprites. Each sprite is usually composed of 3 bytes:
 * yyyyEESY XXXXssss NNNNNNNN
 * yyyy - Y position
 * EE - Extra bits
 * XXXX - X position
 * ssss - Screen number
 * NNNNNNNN - Sprite ID (or command)
 * Last byte: end of data. Should be 0xFF.
 * The sprite data, however, can change depending on the size of the sprite. If the
 * sprite size is > 3, then any other byte after the first 3 are "extension bytes".
 * Example:
 *
 */

/*
 * The mwt format is really simple: Each line is the name of the sprite (yes, including the sprite ID). 
 * These are used when...
 */

/*
 * The ssc format is a text format containing lines formatted like this:
 *   (sprite id)(space)(extra bits and line type)(space)(rest of line, either describing tooltip or sprite tiles)
 * Example:
 *   04  20  A normal thwomp.
 *   04  22  0,0,34  0,16,44  16,0,35  16,16,45
 * The second column indicates whether the line contains a tooltip or sprite tiles:
 *   - If the last digit == 0, then the line contains a tooltip;
 *   - If it's == 2, then it contains sprite tiles;
 * This feature was presumabily made so that it could later be extended.
 */

#ifndef _SPRITE_FILES_H
#define _SPRITE_FILES_H

#include "sprite.h"

class QString;

/* Simultaneously reads both the mw2 file and mwt file and returns the following error codes:
 *  - 1: The mw2 or mwt file does not exist (or can't open).
 *  - 2: The mw2 file has a bad format;
 *  - 3: The mwt file is longer than the mw2 file (I.E. has more lines and more possible information);
 *  - 4: The mw2 file is longer than the mwt file. In this case, all information found in the mw2 file should be kept;
 * This function also inserts sprites in the global sprite structure; therefore, make sure to call "load_size_table"
 * before calling this function, and, if a fatal error is found, to clear the sprite structure. */
int mw2_mwt_readfile(sprite::SpriteMap &spmap, const QString &romname);

/* Reads the ssc file and updates the sprites. Returns 1 on format error. */
int ssc_readfile(sprite::SpriteMap &spmap, const QString &romname);

/* Writes to the mw2 file. Returns 1 for file error. */
int mw2_writefile(sprite::SpriteMap &spmap, const QString &romname);

/* Writes to the mwt file. Returns 1 for file error. */
int mwt_writefile(sprite::SpriteMap &spmap, const QString &romname);

/* Writes to the ssc file. Returns 1 for file error. */
int ssc_writefile(sprite::SpriteMap &spmap, const QString &romname);



struct Maptile;

/* 
 * s16 file format:
 * 43 2D 53 2D 44 2D 54 2D
 * First byte: 43: Tile number, offset into sprite tile map
 * Second byte: 2D: YXPPCCCT ?
 */
int s16_readfile(Maptile arrtiles[0x2000], char *romname);
int s16_writefile(Maptile arrtiles[0x2000], char *romname);

#endif

