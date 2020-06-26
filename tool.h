#ifndef _TOOL_H_
#define _TOOL_H_

#include <QObject>
#include <QMultiMap>
#include "sprite.h"
#include "sprite_defines.h"
#include "ext/libsmw.h"

class QIcon;

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
    bool _open = false;
    bool _unsaved = false;
    unsigned int inserted[SPRITE_ID_MAX];
    unsigned int _inserted_count = 0;

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

    bool unsaved() const
    {
        return _unsaved;
    }

    bool is_open() const
    {
        return _open;
    }

    const unsigned int *inserted_sprites() const
    {
        return inserted;
    }

    unsigned int inserted_count() const
    {
        return _inserted_count;
    }

    int open(const QString &rompath, QString &errors);
    void close(void);
    void save();
    bool update_sprite(const sprite::SpriteKey &key, const sprite::SpriteValue &oldvalue,
            const sprite::SpriteValue &newvalue);
    int insert_sprite(const sprite::SpriteKey &key, const sprite::SpriteValue &val);
    void remove_sprite(sprite::SpriteKey &key, sprite::SpriteValue &val);
    bool is_in_map(int id);
    const QIcon &get_icon(const sprite::SpriteKey &key, const sprite::SpriteValue &val);
};

#endif
