#ifndef _TOOL_H_
#define _TOOL_H_

#include <QObject>
#include "sprite.h"
#include "ext/libsmw.h"

/* What is this:
 * This is a half-assed implementation of a Singleton, which means it should be a class with only
 * one instance, expect you can *technically* create more instances. I don't care about it, though,
 * since it could be later extended to allow to work on more ROMs at one time.
 * What this actually does:
 * Maintains the state for one ROM "buffer". Or in other words: the rom struct and the sprite list
 * is maintained here, in what should be a safe place (although it isn't technically safe yet). */
class Tool : public QObject {
private:
    sprite::SpriteMap _sprite_map;
    smw::ROM main_rom;
    QString rom_filename;
    bool not_open = true;
    bool unsaved = false;

public:
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

    ~Tool()
    {
        close();
    }

    const sprite::SpriteMap &sprite_map(void) const
    {
        return _sprite_map;
    }

    int open(const QString &rompath, QString &errors);
    void close(void);
};

#endif
