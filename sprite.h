/* sprite.h: not exactly a sprite class comprising every information about
 * sprites in super mario world. */

#include <QString>

class Sprite {
public:
    QString name;
    QString tooltip;
    unsigned char command;
    unsigned char extrabytes;
    
    Sprite()
    { 
        command = 0;
        extrabytes = 0;
    }

    Sprite(QString n, QString t, unsigned char c, unsigned char eb)
    { 
        name = n;
        tooltip = t;
        command = c;
        extrabytes = eb;
    }
};
