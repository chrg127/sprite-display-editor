=== Sprite GFX Tool ===

This tool's name is not intuitive at all. This is a tool that manages a Super Mario World ROM's "custom collection of sprites", that is, it manages
names, commands and graphics as displayed by Lunar Magic.
This tool WON'T insert sprites to the ROM. This is better done by other sprite tools, such as PIXI. It only modifies what Lunar Magic sees, and 
does nothing to the ROM (except for reading specific information).

=== Compiling ===

Sprite GFX Tool uses the QT framework. Install the QT libraries in any way you wish, then use qmake to generate the build files. 
This is simply done using two commands:

    cd /path/to/repository
    qmake


