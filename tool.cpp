#define DEBUG

#include "tool.h"

#include <QString>
#include <QMultiMap>
#include "sprite.h"
#include "sprite_files.h"
#include "ext/libsmw.h"
#include "ext/asar_errors_small.h"

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

    return 0;
}

void Tool::close(void)
{
    _sprite_map.clear();
    smw::closerom(&main_rom, false);
}

