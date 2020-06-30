
#include "tool.h"

#include <QString>
#include <QMultiMap>
#include <QPixmap>
#include <QColor>
#include <QIcon>
#include "sprite_defines.h"
#include "sprite.h"
#include "file_formats.h"
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"
#include "sprite_tools.h"

//#define DEBUG

#ifdef DEBUG
#include <QDebug>
#endif

using sprite::SpriteKey;
using sprite::SpriteValue;

/* opens rom, fills sprite map. Returns 0 for success, 1 for minor error (no need to report),
 * 2 for critical error (should report) */
int Tool::open(const QString &rompath, QString &errors)
{
    int err = 0;
    bool romerr;

    rom_filename = rompath.split(".", QString::SkipEmptyParts).at(0);
    romerr = main_rom.openrom(rompath.toLatin1().data(), false);
    if (!romerr && smw::openromerror == smw::errid_open_rom_failed) {
        errors += "Failed to open ROM";
        return 2;
    }

    sprite::load_size_table(main_rom);

    // Find out how many sprites were inserted in the ROM. (working feature)
    if (sprite::check_pixi_inserted(main_rom) && !sprite::check_pixi_perlevel(main_rom)) {
        sprite::find_pixi_sprites(main_rom, inserted, &_inserted_count);
#ifdef DEBUG
        qDebug() << _inserted_count;
        for (unsigned int i = 0; i < _inserted_count; i++)
            qDebug() << "ID:" << inserted[i];
#endif
    }

    err = mw2_mwt_readfile(_sprite_map, rom_filename);
    // couldn't open files = likely not an user error, since you CAN edit ROMs
    // without using custom .mw2 and .mwt files
    if (err == 1) {
        _sprite_map.clear();
        return 1;
    } else if (err == 2) {
        errors += "Bad format on the .mw2 file. Could not read.";
        _sprite_map.clear();
        return 2;
    }
    err = ssc_readfile(_sprite_map, rom_filename);
    // probably worth just resetting all the tooltips in case of error...
    //errors += "Bad format on the .ssc file. Could not read it.";
    if (err != 0)
        return 1;

    _open = true;
    return 0;
}

void Tool::close(void)
{
    _sprite_map.clear();
    main_rom.closerom(false);
    _open = false;
    _unsaved = false;
}

void Tool::save()
{
    mw2_writefile(_sprite_map, rom_filename);
    mwt_writefile(_sprite_map, rom_filename);
    ssc_writefile(_sprite_map, rom_filename);
}

/* Updates a sprite. This probably could be achieved with a sprite_map.replace() - but I don't
 * like its effects, so this will have to do. */
bool Tool::update_sprite(const sprite::SpriteKey &key, const sprite::SpriteValue &oldvalue,
            const sprite::SpriteValue &newvalue)
{
    auto it = _sprite_map.find(key);
    if (it == _sprite_map.end())
        return false;
    while (it != _sprite_map.end() && it.key() == key && it.value() != oldvalue)
        ;
    if (it == _sprite_map.end() || it.key() != key)
        return false;
    it.value() = newvalue;
    _unsaved = true;

    return true;
}

/* Likewise, just inserting a new sprite won't do. We'd like to check if a similar sprite -
 * one with same ID, extra bits and extension bytes - exists.
 * This is where I start wondering whether implementing a data structure myself would be
 * worth it... */
int Tool::insert_sprite(const sprite::SpriteKey &key, const sprite::SpriteValue &val)
{
    sprite::SpriteValue *tmp;

    tmp = sprite::get_single_value(_sprite_map, key, val.ext_bytes);
    if (tmp != nullptr)
        return 1;
    _sprite_map.insert(key, val);
    return 0;
}

void Tool::remove_sprite(sprite::SpriteKey &key, sprite::SpriteValue &val)
{
    _sprite_map.remove(key, val);
    _unsaved = true;
}

bool Tool::is_in_map(int id)
{
    SpriteKey key(id, 2);
    return _sprite_map.find(key) != _sprite_map.end();
}


const QIcon &Tool::get_icon(const SpriteKey &key, const SpriteValue &val)
{
    static QIcon test_icon;
    static QPixmap pixmap(32, 32);
    static QImage img(32, 32, QImage::Format_RGB32);
    static QColor col;
    static bool done = false;

    if (done)
        return test_icon;
#ifdef DEBUG
    qDebug() << "creating image";
#endif
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (i%2 == 0 || j%2 == 0)
                col.setRgb(0xFF, 0xFF, 0xFF);
            else
                col.setRgb(0, 0, 0);
            img.setPixelColor(i, j, col);
        }
    }

    QPixmap pixmap2 = pixmap.fromImage(img);
#ifdef DEBUG
    qDebug() << pixmap2;
#endif
    test_icon.addPixmap(pixmap2);
    done = true;
    return test_icon;
}

