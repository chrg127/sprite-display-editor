=== Sprite Display Editor ===

This is a tool made to modify the display name, tooltip and graphics of sprites inside Lunar Magic. It can modify both normal sprites and sprites inserted by a custom sprite inserter - such as PIXI.
It's made to be very straightforward to use. Its workflow likely doesn't need any explanation outside of what you might find in Lunar Magic's help file.

=== Compiling ===

Sprite GFX Tool uses the QT framework. Install the QT libraries in any way you wish, then use qmake to generate the build files. 
This is simply done using two commands:

    cd /path/to/repository
    qmake

=== Files ===

main.cpp:               Self explanatory.
tool.cpp:               Manages just about anything that's not the UI.
main_window.cpp:        Program Main Window, manages main UI events.
dialogs.cpp:            The windows opened by the buttons on the main
                        window.
sprite.cpp:             A small library for working with sprites.
sprite_files.cpp:       A small library that manages reading and writing of 
                        some files associated with sprites: .mw2, .mwt, .ssc
                        and .s16. What these files are is described in the
                        .h file.
ext/libsmw.cpp:         A library for managing (specifically) SMW ROMs. Taken
                        from Asar and modified so it could stay standalone.
ext/asar_errors.h:      Errors associated to libsmw.cpp.
crc32.cpp:              The crc32 library.
