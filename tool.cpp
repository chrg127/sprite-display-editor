#include "tool.h"

#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include <QStringList>
#include <cctype>

int Tool::find_sprite(const QString &s)
{
    int i;

    for (i = 0; i < spritelist.size(); i++) {
        if (spritelist[i].name == s)
            return i;
    }
    return -1;
}

int Tool::find_sprite(const unsigned char cmd)
{
    int i;

    for (i = 0; i < spritelist.size(); i++) {
        if (spritelist[i].command == cmd)
            return i;
    }
    return -1;
}

void Tool::swap_sprite(size_t i, size_t n)
{
    Sprite &tmp = spritelist[i+n];
    spritelist[i+n] = spritelist[i];
    spritelist[i] = tmp;
}

int Tool::update_sprite(int i, Sprite &s)
{
    if (i < spritelist.size())
        return 1;
    spritelist[i].name = s.name;
    spritelist[i].tooltip = s.tooltip;
    spritelist[i].command = s.command;
    spritelist[i].extrabytes = s.extrabytes;
    return 0;
}

void Tool::get_sprite_list(const QString &romname)
{
    //Given a filename without extension, open all these files:
    //romname.ssc
    //romname.mw2
    //romname.mwt
}

int open_ssc_file(const QString &romname)
{
    QFile sscfile(romname + ".ssc");
    QTextStream sscstream;
    QStringList strlist;
    int i;
    
    if (!sscfile.exists())
        return 2;
    if (!sscfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;

    sscstream.setDevice(&sscfile);
    while (!sscstream.atEnd()) {
        //assume there already are some sprites in the vector.
        strlist = sscstream.readLine().split('\t', 
                QString::SkipEmptyParts);
        // somehow figure out if the line is about tooltips or not
        if (strlist.size() == 3 && std::isdigit(strlist[2][0]))
            spritelist.tooltip = strlist[2];
        else {
            spritelist.
        }
    }
    sscfile.close();

    return 0;
}


/*
int Tool::get_ssc_values(const QString &line, QString &tooltip,
        unsigned char &command, unsigned char &ebytes)
{
    QStringList strlist = line.split('\t', QString::SkipEmptyParts);
    bool ok;
    command = (unsigned char) strlist[0].toInt(&ok, 16);
    if (!ok)
        return 1;
    ebytes = (unsigned char) strlist[1].toInt(&ok, 16);
    if (!ok)
        return 1;
    tooltip = strlist[2];
    return 0;
}*/
