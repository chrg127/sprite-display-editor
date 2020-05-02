#include "window.h"

#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QListView>
#include <QLineEdit>

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(500, 500);

    addsprite = new QPushButton("Add new sprite", this);
    moveup    = new QPushButton("Move up",        this);
    movedown  = new QPushButton("Move down",      this);
    editlook  = new QPushButton("Edit look",      this);
    spritename = new QLabel("Sprite name: ",    this);
    spritecmd  = new QLabel("Sprite command: ", this);
    spritetip  = new QLabel("Sprite tooltip: ", this);
    spritelist = new QListView(this);
    namebox = new QLineEdit(this);
    cmdbox  = new QLineEdit(this);
    tipbox  = new QLineEdit(this);
    
    // Buttons
    addsprite->setGeometry(190, 450, 120, 30);
    addsprite->setToolTip("Adds sprites.");
    moveup->setGeometry(25, 335, 40, 30);
    moveup->setToolTip("Moves a sprite up in the list.");
    movedown->setGeometry(70, 335, 40, 30);
    movedown->setToolTip("Moves a sprite down in the list.");
    editlook->setGeometry(0, 0, 0, 0);
    editlook->setToolTip("Edit the graphics used by the sprite.");
    // Labels
    spritename->setGeometry(135, 25, 120, 30);
    spritecmd->setGeometry(135, 65, 120, 30);
    spritetip->setGeometry(135, 105, 120, 30);
    // ListView
    spritelist->setGeometry(25, 25, 100, 300);
    // LineEdits
    namebox->setGeometry(265, 25, 120, 30);
    cmdbox->setGeometry(265, 65, 120, 30);
    tipbox->setGeometry(265, 105, 120, 30);

    QObject::connect(addsprite, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));
}

