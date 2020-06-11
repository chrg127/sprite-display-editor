#ifndef _TOOL_H_
#define _TOOL_H_

#include <QDir>
#include "sprite.h"
#include "ext/libsmw.h"

template<class K, class V> class QMultiMap;

/* What is this:
 * This is a half-assed implementation of a Singleton, which means it should be a class with only
 * one instance, expect you can *technically* create more instances. I don't care about it, though,
 * since it could be later extended to allow to work on more ROMs at one time.
 * What this actually does:
 * Maintains the state for one ROM "buffer". Or in other words: the rom struct and the sprite list
 * is maintained here, in what should be a safe place (although it isn't technically safe yet). */
class Tool {
public:
    QMultiMap<sprite::SpriteKey, sprite::SpriteValue> sprite_map;
    smw::ROM main_rom;
    QString rom_filename;
    
    /* libsmw doesn't have any ROM initialization by design. So let's do it here. (if it happens
     * to be more cumbersome down the line, I'll just add a constructor for the ROM). */
    Tool()
    {
        main_rom.file = nullptr;
        main_rom.data = nullptr;
        main_rom.lenght = 0;
        main_rom.mapper = smw::mapper_t::lorom;
        main_rom.header = false;
    }

    ~Tool() { }

    int open(const QString &rompath, QString &errors);
    int save(void);
    int close(void);
};

#endif
