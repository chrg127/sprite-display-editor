#ifndef _TOOL_H_
#define _TOOL_H_

#include <QMap>
#include <QString>
#include "sprite.h"
#include "ext/libsmw.h"

namespace romutils {

int mw2_mwt_readfile(const QString &romname);
//int mwt_readfile(const QString &romname);
//int ssc_readfile(const QString &romname);

}
    
#endif
