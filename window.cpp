#include "window.h"

#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QLabel>
#include <QListView>
#include <QLineEdit>
#include <QStatusBar>
#include <QKeySequence>
#include <QFileDialog>
#include <QString>

#define DEBUG

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(500, 500);
    resize(500, 500);
    center_widget = new QWidget(this);
    setCentralWidget(center_widget);
    statusBar()->showMessage("Hello world!");

    createactions();
    createmenu();
    createbuttons();
    createlabels();
    createlistview();
    createlineedits();
}

void Window::createactions(void)
{
    openact = new QAction("&Open", this);

    openact->setShortcuts(QKeySequence::Open);
    connect(openact, &QAction::triggered, this, &Window::open_file);
}

void Window::createmenu(void)
{
    filemenu = menuBar()->addMenu("&File");
    filemenu->addAction(openact);
    aboutmenu = menuBar()->addMenu("&About");
}

void Window::createbuttons(void)
{
    addspritebtn = new QPushButton("Add new sprite", center_widget);
    moveupbtn    = new QPushButton("Move up",        center_widget);
    movedownbtn  = new QPushButton("Move down",      center_widget);
    editlookbtn  = new QPushButton("Edit look",      center_widget);

    addspritebtn->setGeometry(190, 400, 120, 30);
    moveupbtn->setGeometry(25, 335, 40, 30);
    movedownbtn->setGeometry(70, 335, 40, 30);
    editlookbtn->setGeometry(0, 0, 0, 0);

    addspritebtn->setToolTip("Adds sprites.");
    moveupbtn->setToolTip("Moves a sprite up in the list.");
    movedownbtn->setToolTip("Moves a sprite down in the list.");
    editlookbtn->setToolTip("Edit the graphics used by the sprite.");
}

void Window::createlabels(void)
{
    namelabel = new QLabel("Sprite name: ",    center_widget);
    cmdlabel  = new QLabel("Sprite command: ", center_widget);
    tiplabel  = new QLabel("Sprite tooltip: ", center_widget);
    romnamelabel = new QLabel("ROM name: ",    center_widget);

    namelabel->setGeometry(135, 45, 120, 30);
    cmdlabel->setGeometry(135, 65, 120, 30);
    tiplabel->setGeometry(135, 105, 120, 30);
    romnamelabel->setGeometry(25, 10, 120, 30);
}

void Window::createlistview(void)
{
    spritelistview = new QListView(center_widget);

    spritelistview->setGeometry(25, 45, 100, 300);
}

void Window::createlineedits(void)
{
    namebox = new QLineEdit(center_widget);
    cmdbox  = new QLineEdit(center_widget);
    tipbox  = new QLineEdit(center_widget);

    namebox->setGeometry(265, 25, 120, 30);
    cmdbox->setGeometry(265, 65, 120, 30);
    tipbox->setGeometry(265, 105, 120, 30);
}
/*    QObject::connect(addsprite, SIGNAL(clicked()), obj, 
            SLOT(dostuff()));
    QObject::connect(addsprite, SIGNAL(clicked(bool)), this,
            SLOT(buttonclicked(bool)));
    QObject::connect(this, SIGNAL(counter_reached()), 
            QApplication::instance(), SLOT(quit()));*/

void Window::open_file(bool checked)
{
    QString filename;

#ifdef DEBUG
    qDebug() << "received a request to open a file\n" << "checked: " 
           << checked << '\n';
#endif
    filename = QFileDialog::getOpenFileName(this, "Open Image", "/", 
            "SNES ROM files (*.smc *.sfc)");
#ifdef DEBUG
    qDebug() << "Chose file " << filename;
#endif
    romnamelabel->setText("ROM name: " + filename);
}

