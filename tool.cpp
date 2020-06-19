#define DEBUG

#include "tool.h"

#include <QString>
#include <QMultiMap>
#include "sprite.h"
#include "sprite_files.h"
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"

#include <QDebug>

/* opens rom, fills sprite map. Returns 0 for success, 1 for minor error (no need to report),
 * 2 for critical error (should report) */
int Tool::open(const QString &rompath, QString &errors)
{
    int err = 0;
    bool romerr;

    rom_filename = rompath.split(".", QString::SkipEmptyParts).at(0);
    romerr = smw::openrom(&main_rom, rompath.toLatin1().data(), false);
    if (!romerr && smw::openromerror == smw::errid_open_rom_failed) {
        errors += "Failed to open ROM";
        return 2;
    }
    sprite::load_size_table(main_rom);
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
    smw::closerom(&main_rom, false);
    _open = false;
    _unsaved = false;
}

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

void Tool::insert_sprite(const sprite::SpriteKey &key, const sprite::SpriteValue &val)
{
    _sprite_map.insert(key, val);
}

void Tool::save()
{
    mw2_writefile(_sprite_map, rom_filename);
    mwt_writefile(_sprite_map, rom_filename);
    ssc_writefile(_sprite_map, rom_filename);
}

