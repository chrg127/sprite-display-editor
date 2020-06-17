#define DEBUG

#include "main_window.h"

#include <QPushButton>
#include <QMenuBar>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include "sprite.h"
#include "tool.h"

#include <QVariant>

#ifdef DEBUG
#include <QDebug>
#include <iostream>
#endif

MainWindow::MainWindow(Tool *tool, QWidget *parent)
    : QMainWindow(parent), main_tool(tool), last_dir(QDir::homePath())
{
    QWidget *center_widget      = new QWidget(this);
    QPushButton *addspritebtn   = new QPushButton("Add new sprite");
    QHBoxLayout *buttonlt       = new QHBoxLayout;
    QVBoxLayout *mainlt         = new QVBoxLayout(center_widget);

    create_menu();
    create_buttons(buttonlt);

    center_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    addspritebtn->setToolTip("Adds sprites.");

    mainlt->addWidget(romnamelabel);
    mainlt->addWidget(sprite_list);
    mainlt->addLayout(buttonlt);
    mainlt->addWidget(addspritebtn, 0, Qt::AlignHCenter);

    setMinimumWidth(250);
    setWindowTitle("spritegfxtool");
    setCentralWidget(center_widget);

    connect(addspritebtn, &QAbstractButton::released, this, &MainWindow::add_new_sprite);
    //connect(sprite_list, &QListWidget::currentItemChanged, this, &MainWindow::item_changed);
}

void MainWindow::create_menu(void)
{
    QMenu *menu;
    QAction *act;

    menu = menuBar()->addMenu("&File");
    act = new QAction("&Open", this);
    connect(act, &QAction::triggered, this, &MainWindow::open_file);
    menu->addAction(act);
    menu = menuBar()->addMenu("&About");
}

void MainWindow::create_buttons(QHBoxLayout *lt)
{
    QPushButton *editsprite     = new QPushButton("Edit sprite");
    QPushButton *editlook       = new QPushButton("Edit look");
    QPushButton *removesprite   = new QPushButton("Remove sprite");

    editsprite->setToolTip("Edit the selected sprite.");
    editlook->setToolTip("Edit the graphics used by the selected sprite.");
    removesprite->setToolTip("Removes the selected sprite.");

    lt->addWidget(new QWidget, 0, Qt::AlignLeft);
    lt->addWidget(editsprite);
    lt->addWidget(editlook);//, 0, Qt::AlignHCenter);
    lt->addWidget(removesprite);//, 0, Qt::AlignHCenter);
    lt->addWidget(new QWidget, 0, Qt::AlignRight);

    connect(editsprite, &QAbstractButton::released, this, &MainWindow::edit_sprite);
    connect(editlook, &QAbstractButton::released, this, &MainWindow::edit_look);
    connect(removesprite, &QAbstractButton::released, this, &MainWindow::remove_sprite);
}



void MainWindow::open_file()
{
    QString name, errors;
    QMessageBox msg;
    int err;
    QString item_msg;
    QListWidgetItem *item;
    sprite::SpritePair sp;

    name = QFileDialog::getOpenFileName(this, "Open Image", last_dir,
            "SNES ROM files (*.smc *.sfc)");
    if (name == "")
        return;
    err = main_tool->open(name, errors);
    if (err == 2) {
        msg.setText("Found errors:\n" + errors);
        msg.exec();
        return;
    }

    // No errors found, setup the widgets
    last_dir = name;
    romnamelabel->setText("ROM name: " + QFileInfo(name).fileName());
    const sprite::SpriteMap &spmap = main_tool->sprite_map();
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
        if (it.value().name != "")
            item_msg = it.value().name;
        else
            item_msg = QString("ID: %1; extra bits: %2").arg(it.key().id).arg(it.key().extra_bits());
        item = new QListWidgetItem(item_msg, sprite_list);
        sp.k = &it.key();
        sp.v = &it.value();
        item->setData(Qt::UserRole, QVariant::fromValue(sp));
    }
}

void MainWindow::add_new_sprite()
{
    add_dialog->exec();
}

void MainWindow::edit_sprite()
{
    edit_dialog->exec();
}

void MainWindow::edit_look()
{
#ifdef DEBUG
    qDebug() << "Selected \"edit look\" button";
#endif
}

void MainWindow::remove_sprite()
{
#ifdef DEBUG
    qDebug() << "Selected \"remove sprite\" button";
#endif
}

/*
void MainWindow::item_changed(QListWidgetItem *curr, QListWidgetItem *prev)
{
    sprite::SpritePair sp;
    QString extstr = "";
    int i;

    if (curr != NULL) {
        sp = curr->data(Qt::UserRole).value<sprite::SpritePair>();
        idlabel->setText(QString("%1").arg(sp.k->id));
        eblabel->setText(QString("%2").arg(sp.k->extra_bits()));
        namebox->setText(sp.v->name);
        tipbox->setText(sp.v->tooltip);
        for (i = 0; i < sp.k->get_ext_size(); i++)
            extstr += QString("%1").arg(sp.v->ext_bytes[i]);
        extbox->setText(extstr);
    }
}*/

