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
    bool openerr;
    QString path_noext;

    rom_filename = rompath;
    openerr = smw::openrom(&main_rom, rompath.toLatin1().data(), false);
    if (!openerr && smw::openromerror == smw::errid_open_rom_failed) {
        errors += "Failed to open the ROM";
        return 2;
    }
    sprite::load_size_table(main_rom);

    path_noext = rompath.split(".", QString::SkipEmptyParts).at(0);
    err = mw2_mwt_readfile(_sprite_map, path_noext);
    if (err == 1) {
        _sprite_map.clear();
        return 1;
    } else if (err == 2) {
        errors += "Bad format on the .mw2 or .mwt file. Could not read it.";
        _sprite_map.clear();
        return 2;
    }
    err = ssc_readfile(_sprite_map, path_noext);
    if (err != 0) {
        errors += "Bad format on the .ssc file. Could not read it.";
        return 2;
    }
    return 0;
}

void Tool::close(void)
{
    _sprite_map.clear();
    smw::closerom(&main_rom, false);
}

/*    case 3:
        errors += "The .mwt file references sprites not recorded into the .mw2 file.";
        break;
    case 4:
        errors += "The .mw2 file has records of sprites not yet recorded into the .mwt file.\n";*/
/*
int Tool::save(void)
{
    mw2_writefile(_sprite_map, rom_filename);
    mwt_writefile(_sprite_map, rom_filename);
    ssc_writefile(_sprite_map, rom_filename);
    return 0;
}
void Tool::add_sprite(const sprite::SpriteKey &sk, const sprite::SpriteValue &sv)
{
    _sprite_map.insert(sk, sv);
}
*/


