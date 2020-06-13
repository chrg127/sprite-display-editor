#define DEBUG

#include "main_window.h"

#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include "sprite.h"
#include "tool.h"

#ifdef DEBUG
#include <QDebug>
#include <iostream>
#endif

MainWindow::MainWindow(Tool *tool, QWidget *parent)
    : QMainWindow(parent), main_tool(tool), last_dir(QDir::homePath())
{
    QVBoxLayout *mainlt      = new QVBoxLayout(center_widget);
    QHBoxLayout *label_lt    = new QHBoxLayout;
    QFormLayout *edit_formlt = new QFormLayout;
    QHBoxLayout *list_mainlt = new QHBoxLayout;
    QHBoxLayout *button_mainlt = new QHBoxLayout;

    setMinimumWidth(250);
    setWindowTitle("spritegfxtool");
    setCentralWidget(center_widget);
    center_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

    label_lt->addWidget(romnamelabel);
    create_menu();
    create_edit_zone(edit_formlt);
    create_lists(list_mainlt);
    create_buttons(button_mainlt);

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

void MainWindow::create_edit_zone(QFormLayout *flt)
{
    flt->addRow(namelabel, namebox);
    flt->addRow(idlabel, idbox);
    flt->addRow(tiplabel, tipbox);
}

void MainWindow::create_buttons(QHBoxLayout *lt)
{
    editlookbtn->setToolTip("Edit the graphics used by the selected sprite.");
    addspritebtn->setToolTip("Adds sprites.");
    removespritebtn->setToolTip("Removes the selected sprite.");

    lt->addWidget(new QWidget, 0, Qt::AlignLeft);
    lt->addWidget(editlookbtn);//, 0, Qt::AlignHCenter);
    lt->addWidget(removespritebtn);//, 0, Qt::AlignHCenter);
    lt->addWidget(new QWidget, 0, Qt::AlignRight);

    connect(editlookbtn, &QAbstractButton::clicked, this, &MainWindow::edit_look);
    connect(addspritebtn, &QAbstractButton::clicked, this, &MainWindow::add_new_sprite);
    connect(removespritebtn, &QAbstractButton::clicked, this, &MainWindow::remove_sprite);
}

void MainWindow::create_lists(QHBoxLayout *lt)
{
    lt->addWidget(filled_sprite_list);
    lt->addWidget(unfilled_sprite_list);
}

MainWindow::~MainWindow()
{
    main_tool->close();
}



/* ************ Slots ************ */
void MainWindow::open_file(bool checked)
{
    QString name, name_noext, errors;
    QMessageBox msg;
    int err;
    QListWidgetItem *item;

    name = QFileDialog::getOpenFileName(this, "Open Image", last_dir,
            "SNES ROM files (*.smc *.sfc)");
    if (name == "")
        return;
    romnamelabel->setText("ROM name: " + QFileInfo(name).fileName());
    last_dir = name;
    err = main_tool->open(name, errors);
    if (err == 2) {
        msg.setText("Found errors:\n" + errors);
        msg.exec();
    }

    /*
    item = new QListWidgetItem(QIcon("./stuff/icon64.png"), "test", filled_sprite_list);
    item2 = new QListWidgetItem(QIcon("./stuff/icon64.png"), "test2", filled_sprite_list);*/
    const sprite::SpriteMap &spmap = main_tool->sprite_map();
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
        item = new QListWidgetItem(it.value().name, filled_sprite_list);
    }
}

void MainWindow::add_new_sprite(bool checked)
{
    int ret = add_dialog->exec();
    if (ret == 0)
        return;
}

void MainWindow::remove_sprite(bool checked)
{
    qDebug() << "Selected \"remove sprite\" button";
}

void MainWindow::edit_look(bool checked)
{
    qDebug() << "Selected \"edit look\" button";
}

/*
void MainWindow::btadd_func(bool checked)
{
    qDebug() << "successfully got here";
}*/
