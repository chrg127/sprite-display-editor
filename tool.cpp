#include "tool.h"

#include <QString>
#include <QMultiMap>
#include "sprite_files.h"
//#include "sprite.h"
//#include "ext/libsmw.h"

/* opens rom, fills sprite map. Returns 0 for success, 1 for mild error, 2 for critical error */
int Tool::open(const QString &rompath, QString &errors)
{
    int err = 0;
    
    rom_filename = rompath;
    smw::openrom(&main_rom, rompath.toLatin1().data(), false);
    sprite::load_size_table(main_rom);
    err = mw2_mwt_readfile(sprite_map, rompath);
    switch (err) {
    case 1:
        errors += "Warning: the .mw2 or the .mwt file does not exist\n";
        break;
    case 2:
        errors += "Warning: the .mw2 file has a bad format.\n";
        break;
    case 3:
        errors += "Warning: some shit \n";
        break;
    case 4:
        errors += "Warning: some other shit\n";
    }

    if (err != 1) {
        err = ssc_readfile(sprite_map, rompath);
        if (err != 0)
            errors += "Warning: some other shit about ssc\n";
    }
    sprite::print_sprites(sprite_map);
    
    return err == 0 ? 0 : 1;
}

int Tool::save(void)
{
    mw2_writefile(sprite_map, rom_filename);
    mwt_writefile(sprite_map, rom_filename);
    ssc_writefile(sprite_map, rom_filename);
    return 0;
}

int Tool::close(void)
{
    sprite_map.clear();
    smw::closerom(&main_rom, false);
    return 0;
}
