#include "main_window.h"

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
#include <QListWidget>
#include <QLineEdit>
#include <QStatusBar>
#include <QKeySequence>
#include <QFileDialog>
#include <QString>
#include <QDir>
#include "sprite.h"

#define DEBUG

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(500, 500);
    resize(500, 500);
    center_widget = new QWidget(this);
    setCentralWidget(center_widget);
    statusBar()->showMessage("Hello world!");
    last_dir = QDir::homePath();

    createmenu();
    createbuttons();
    createlabels();
    createlistwidget();
    createlineedits();
}

void MainWindow::createmenu(void)
{
    QAction *act;

    filemenu = menuBar()->addMenu("&File");
    act = new QAction("&Open", this);
    connect(act, &QAction::triggered, this, &MainWindow::open_file);
    filemenu->addAction(act);
    aboutmenu = menuBar()->addMenu("&About");
}

void MainWindow::createbuttons(void)
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

void MainWindow::createlabels(void)
{
    namelabel = new QLabel("Sprite name: ",    center_widget);
    cmdlabel  = new QLabel("Sprite command: ", center_widget);
    tiplabel  = new QLabel("Sprite tooltip: ", center_widget);
    romnamelabel = new QLabel("ROM name: ",    center_widget);

    namelabel->setGeometry(135, 45, 120, 30);
    cmdlabel->setGeometry(135, 65, 120, 30);
    tiplabel->setGeometry(135, 105, 120, 30);
    romnamelabel->setGeometry(25, 10, 150, 30);
}

void MainWindow::createlistwidget(void)
{
    spritelistwidget = new QListWidget(center_widget);
    spritelistwidget->setGeometry(25, 45, 100, 300);
}

void MainWindow::createlineedits(void)
{
    namebox = new QLineEdit(center_widget);
    cmdbox  = new QLineEdit(center_widget);
    tipbox  = new QLineEdit(center_widget);

    namebox->setGeometry(265, 25, 120, 30);
    cmdbox->setGeometry(265, 65, 120, 30);
    tipbox->setGeometry(265, 105, 120, 30);
}



/* ************ Slots ************
 * open_file: opens the 4 files associated to the ROM and creates the sprite list
 * close_file: clears the sprite list (and associated widgets). doesn't save anything.
 * save_file: saves everything to files
 */
void MainWindow::open_file(bool checked)
{
    QString name;

    name = QFileDialog::getOpenFileName(this, "Open Image", last_dir, 
            "SNES ROM files (*.smc *.sfc)");
#ifdef DEBUG
    qDebug() << "filename: " << name;
#endif
    if (name == "")
        return;
    
    romnamelabel->setText("ROM name: " + QFileInfo(name).fileName());
    last_dir = name;

    /*spritelist = get_sprite_list();
    if ((*spritelist).size() == 0) {
        //QMessageBox::critical(this, "No sprite found."
        return;
    }

    for (auto &s : *spritelist)
        spritelistwidget->addItem(s.name);*/
}

void MainWindow::close_file(bool checked)
{
}

void MainWindow::save_file(bool checked)
{
}

