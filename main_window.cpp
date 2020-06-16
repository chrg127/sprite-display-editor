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
    QVBoxLayout *mainlt         = new QVBoxLayout(center_widget);
    QHBoxLayout *label_lt       = new QHBoxLayout;
    QFormLayout *edit_formlt    = new QFormLayout;
    QHBoxLayout *list_mainlt    = new QHBoxLayout;
    QHBoxLayout *button_mainlt  = new QHBoxLayout;


    setMinimumWidth(250);
    setWindowTitle("spritegfxtool");
    setCentralWidget(center_widget);
    create_menu();
    create_buttons(button_mainlt);
    connect(addspritebtn, &QAbstractButton::clicked, this, &MainWindow::add_new_sprite);
    connect(sprite_list, &QListWidget::currentItemChanged, this, &MainWindow::item_changed);

    center_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    addspritebtn->setToolTip("Adds sprites.");

    label_lt->addWidget(romnamelabel);
    edit_formlt->addRow(new QLabel("ID:"),              idlabel);
    edit_formlt->addRow(new QLabel("Extra bits:"),      eblabel);
    edit_formlt->addRow(new QLabel("Sprite name:"),     namebox);
    edit_formlt->addRow(new QLabel("Sprite tooltip:"),  tipbox);
    edit_formlt->addRow(new QLabel("Extension bytes:"), extbox);
    list_mainlt->addWidget(sprite_list);
    mainlt->addLayout(label_lt);
    mainlt->addWidget(romnamelabel);
    mainlt->addLayout(list_mainlt);
    mainlt->addLayout(edit_formlt);
    mainlt->addLayout(button_mainlt);
    mainlt->addWidget(addspritebtn, 0, Qt::AlignHCenter);
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
    QPushButton *editlookbtn        = new QPushButton("Edit look");
    QPushButton *removespritebtn    = new QPushButton("Remove sprite");

    editlookbtn->setToolTip("Edit the graphics used by the selected sprite.");
    removespritebtn->setToolTip("Removes the selected sprite.");

    lt->addWidget(new QWidget, 0, Qt::AlignLeft);
    lt->addWidget(editlookbtn);//, 0, Qt::AlignHCenter);
    lt->addWidget(removespritebtn);//, 0, Qt::AlignHCenter);
    lt->addWidget(new QWidget, 0, Qt::AlignRight);

    connect(editlookbtn, &QAbstractButton::clicked, this, &MainWindow::edit_look);
    connect(removespritebtn, &QAbstractButton::clicked, this, &MainWindow::remove_sprite);
}


/*
struct SpriteData {
    unsigned char id;
    unsigned char eb;
    unsigned char ext_bytes[SPRITE_MAX_DATA_SIZE];
    SpriteData() : id(0), eb(0)
    {
        for (int i = 0; i < SPRITE_MAX_DATA_SIZE; i++)
            ext_bytes[i] = 0;
    }

};

Q_DECLARE_METATYPE(SpriteData);
*/

void MainWindow::open_file(bool checked)
{
    QString name, name_noext, errors;
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

void MainWindow::add_new_sprite(bool checked)
{
    add_dialog->exec();
}

void MainWindow::remove_sprite(bool checked)
{
#ifdef DEBUG
    qDebug() << "Selected \"remove sprite\" button";
#endif
}

void MainWindow::edit_look(bool checked)
{
#ifdef DEBUG
    qDebug() << "Selected \"edit look\" button";
#endif
}

void MainWindow::item_changed(QListWidgetItem *curr, QListWidgetItem *prev)
{
    sprite::SpritePair sp;
    QString extstr = "";
    int i;

    if (curr != NULL) {
        /*qDebug() << "current:" << curr->text();
        sk = curr->data(Qt::UserRole).value<sprite::SpriteKey>();
        qDebug() << "ID:" << sk.id << "Extra bits:" << sk.extra_bits();*/
        /*sk = curr->data(Qt::UserRole).value<sprite::SpriteKey>();
        sv
        namebox->setText();*/
        sp = curr->data(Qt::UserRole).value<sprite::SpritePair>();
        idlabel->setText(QString("%1").arg(sp.k->id));
        eblabel->setText(QString("%2").arg(sp.k->extra_bits()));
        namebox->setText(sp.v->name);
        tipbox->setText(sp.v->tooltip);
        for (i = 0; i < sp.k->get_ext_size(); i++)
            extstr += QString("%1").arg(sp.v->ext_bytes[i]);
        extbox->setText(extstr);
    }
    /*if (prev != NULL) {
        qDebug() << "prev:" << prev->text();
        sk = prev->data(Qt::UserRole).value<sprite::SpriteKey>();
        qDebug() << "ID:" << sk.id << "Extra bits:" << sk.extra_bits();
    }*/
}

