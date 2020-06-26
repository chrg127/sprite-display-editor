#define DEBUG

#include "main_window.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QVariant>
#include <QCloseEvent>
#include <QList>
#include "dialogs.h"
#include "sprite.h"
#include "tool.h"
#include "version.h"

#ifdef DEBUG
#include <QDebug>
#include <cassert>
#endif

using sprite::SpriteKey;
using sprite::SpriteValue;

MainWindow::MainWindow(Tool *tool, QWidget *parent)
    : QMainWindow(parent), main_tool(tool), last_dir(".")//QDir::homePath())
{
    setMinimumWidth(400);
    setWindowTitle(QStringLiteral("Sprite Display Editor"));

    QWidget *center_widget  = new QWidget(this);
    QHBoxLayout *labellt    = new QHBoxLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;
    QVBoxLayout *mainlt     = new QVBoxLayout(center_widget);
    sprite_list             = new QListWidget;
    add_dialog              = new AddSpriteDialog(tool, this);
    edit_dialog             = new EditSpriteDialog(this);    
    display_dialog          = new EditDisplayDialog(this);

    center_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setCentralWidget(center_widget);
    create_menu();
    create_labels(labellt);
    create_buttons(buttonlt);

    mainlt->addWidget(romnamelabel);
    mainlt->addWidget(sprite_list);
    mainlt->addLayout(labellt);
    mainlt->addLayout(buttonlt);
    mainlt->addWidget(addspritebtn, 0, Qt::AlignHCenter);

    connect(sprite_list, &QListWidget::currentItemChanged, this, &MainWindow::item_changed);
    connect(sprite_list, &QListWidget::itemDoubleClicked, this, &MainWindow::edit_sprite);

    // Disable buttons, menu actions, etc.
    enable_disable(false);
}



enum Menus : int {
    FILE_MENU = 0,
    EDIT_MENU,
    ABOUT_MENU,
};

/* NOTE: Private functions */
void MainWindow::create_menu()
{
    QAction *act;

    menus[FILE_MENU] = menuBar()->addMenu("&File");
    menus[FILE_MENU]->addAction("&Open ROM",  this, SLOT(open_file()));
    menus[FILE_MENU]->addAction("&Close ROM", this, SLOT(close_file()));

    menus[EDIT_MENU] = menuBar()->addMenu("&Edit");
    menus[EDIT_MENU]->addAction("&Add new sprite",      this, SLOT(add_sprite()));
    menus[EDIT_MENU]->addAction("&Edit Sprite",         this, SLOT(edit_sprite()));
    menus[EDIT_MENU]->addAction("Edit &Display Graphics", this, SLOT(edit_look()));
    menus[EDIT_MENU]->addAction("&Remove Sprite",       this, SLOT(remove_sprite()));

    menus[ABOUT_MENU] = menuBar()->addMenu("&About");
    act = menus[ABOUT_MENU]->addAction("&Version");
    connect(act, &QAction::triggered, display_version);
}

void MainWindow::create_labels(QHBoxLayout *lt)
{
    QLabel *tmp     = new QLabel(QStringLiteral("ID:"));
    QLabel *tmp2    = new QLabel(QStringLiteral("Extra bits:"));
    romnamelabel    = new QLabel(QStringLiteral("ROM name:"));
    idlabel         = new QLabel;
    eblabel         = new QLabel;
    tmp->setAlignment(Qt::AlignRight);
    tmp2->setAlignment(Qt::AlignRight);
    idlabel->setAlignment(Qt::AlignLeft);
    eblabel->setAlignment(Qt::AlignLeft);
    lt->addWidget(tmp);
    lt->addWidget(idlabel);
    lt->addWidget(tmp2);
    lt->addWidget(eblabel);
}

void MainWindow::create_buttons(QHBoxLayout *lt)
{
    editsprite     = new QPushButton(QStringLiteral("Edit sprite"));
    editlook       = new QPushButton(QStringLiteral("Edit look"));
    removesprite   = new QPushButton(QStringLiteral("Remove sprite"));
    addspritebtn   = new QPushButton(QStringLiteral("Add new sprite"));

    addspritebtn->setToolTip(QStringLiteral("Add a new sprite."));
    editsprite->setToolTip(QStringLiteral("Edit the selected sprite."));
    editlook->setToolTip(QStringLiteral("Edit the display graphics used by the selected sprite."));
    removesprite->setToolTip(QStringLiteral("Removes the selected sprite."));

    addspritebtn->setEnabled(false);
    editsprite->setEnabled(false);
    editlook->setEnabled(false);
    removesprite->setEnabled(false);

    editsprite->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    editlook->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    removesprite->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    lt->addWidget(new QWidget, 0, Qt::AlignLeft);
    lt->addWidget(editsprite);
    lt->addWidget(editlook);//, 0, Qt::AlignHCenter);
    lt->addWidget(removesprite);//, 0, Qt::AlignHCenter);
    lt->addWidget(new QWidget, 0, Qt::AlignRight);

    connect(addspritebtn, &QAbstractButton::released, this, &MainWindow::add_sprite);
    connect(editsprite, &QAbstractButton::released, this, &MainWindow::edit_sprite);
    connect(editlook, &QAbstractButton::released, this, &MainWindow::edit_look);
    connect(removesprite, &QAbstractButton::released, this, &MainWindow::remove_sprite);
}

QListWidgetItem *MainWindow::add_list_item(const sprite::SpriteKey &key, const sprite::SpriteValue &val,
        int pos)
{
    QString item_msg;
    QListWidgetItem *item;

    if (!val.name.isEmpty())
        item_msg = val.name;
    else
        item_msg = QString("ID: %1; Extra bits: %2")
            .arg(key.id, 2, 16, QLatin1Char('0'))
            .arg(key.extra_bits(), 0, 16);
    item = new QListWidgetItem(item_msg);
    item->setData(Qt::UserRole, QVariant::fromValue(key));
    item->setData(Qt::UserRole+1, QVariant::fromValue(val));

    if (pos == -1)
        sprite_list->addItem(item);
    else
        sprite_list->insertItem(pos, item);
    return item;
}

/* This is worth optimizing later. */
QListWidgetItem *MainWindow::find_item(const sprite::SpriteKey &key, const sprite::SpriteValue &val)
{
    int i, val_equal = 1;

    for (i = 0; i < sprite_list->count(); i++) {
        sprite::SpriteKey tmpkey(sprite_list->item(i)->data(Qt::UserRole).value<sprite::SpriteKey>());
        sprite::SpriteValue tmpval(sprite_list->item(i)->data(Qt::UserRole+1).value<sprite::SpriteValue>());
        for (int j = 0; j < key.get_ext_size() && val_equal == 1; j++)
            if (tmpval.ext_bytes[j] != val.ext_bytes[j])
                val_equal = 0;
        if (tmpkey == key && val_equal == 1)
            return sprite_list->item(i);
    }
    return nullptr;
}

void MainWindow::enable_disable(bool enable)
{
    addspritebtn->setEnabled(enable);
    editsprite->setEnabled(enable);
    editlook->setEnabled(enable);
    removesprite->setEnabled(enable);
    for (int i = 0; i < 4; i++)
        menus[EDIT_MENU]->actions()[i]->setEnabled(enable);
    menus[FILE_MENU]->actions()[0]->setEnabled(!enable);
    menus[FILE_MENU]->actions()[1]->setEnabled(enable);
}



/* NOTE: Static function (mostly messages functions */
static inline int not_open_message()
{
    QMessageBox msg;
    msg.setText(QStringLiteral("No ROM opened."));
    msg.setInformativeText(QStringLiteral("Open a ROM first!"));
    return msg.exec();
}

static inline int no_selected_message()
{
    QMessageBox msg;
    msg.setText(QStringLiteral("No sprites selected."));
    msg.setInformativeText(QStringLiteral("Select a sprite first to use this."));
    return msg.exec();
}

/* Common messages */
static inline int display_save_message()
{
    QMessageBox msg;
    msg.setText(QStringLiteral("There are unsaved changes."));
    msg.setInformativeText(QStringLiteral("Do you want to save your changes?"));
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Save);
    return msg.exec();
}



/* NOTE: Slots */
void MainWindow::add_sprite()
{
    int ret;
    QMessageBox msg;

    add_dialog->clear_fields();
    if (add_dialog->exec() == 0)
        return; // no sprite to add
    sprite::SpriteKey key(add_dialog->getid(), add_dialog->geteb());
    sprite::SpriteValue val;
    val.name = add_dialog->getname();
    val.tooltip = add_dialog->gettip();
    val.str2extb(add_dialog->get_ext(), key.get_ext_size());
    ret = main_tool->insert_sprite(key, val);
    if (ret != 1) {
        sprite_list->setCurrentItem(add_list_item(key, val));
        return;
    }
    msg.setText(QStringLiteral("A similar sprite was found."));
    msg.setInformativeText(QStringLiteral("Would you like to edit that instead?"));
    msg.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msg.setDefaultButton(QMessageBox::Yes);
    ret = msg.exec();
    if (ret != QMessageBox::Yes) {
        sprite_list->setCurrentItem(add_list_item(key, val));
        return;
    }
    QListWidgetItem *item = find_item(key, val);
    sprite_list->setCurrentItem(item);
    editsprite->click();
}

void MainWindow::edit_sprite()
{
    QListWidgetItem *item = sprite_list->currentItem();
    QMessageBox msg;
    QString tmp;

    if (item == nullptr) {
        no_selected_message();
        return;
    }

    // Fill boxes from the list widget item's respective sprite
    sprite::SpriteKey sk(item->data(Qt::UserRole).value<sprite::SpriteKey>());
    sprite::SpriteValue sv(item->data(Qt::UserRole+1).value<sprite::SpriteValue>());
    edit_dialog->init_fields(sk, sv);
    if (edit_dialog->exec() == 0)
        return;

    // get text from boxes and update everything.
    sprite::SpriteValue newsv(sv);
    newsv.name = edit_dialog->getname();
    newsv.tooltip = edit_dialog->gettip();
    newsv.str2extb(edit_dialog->get_ext(), sk.get_ext_size());

    main_tool->update_sprite(sk, sv, newsv);

    item->setText(newsv.name);
    item->setData(Qt::UserRole+1, QVariant::fromValue(newsv));
    if (main_tool->unsaved())
        romnamelabel->setText(romnamelabel->text()+'*');
}

void MainWindow::edit_look()
{
    display_dialog->exec();
}

void MainWindow::remove_sprite()
{
    QListWidgetItem *item = sprite_list->currentItem();
    if (item == nullptr) {
        no_selected_message();
        return;
    }
    sprite::SpriteKey key(item->data(Qt::UserRole).value<sprite::SpriteKey>());
    sprite::SpriteValue val(item->data(Qt::UserRole+1).value<sprite::SpriteValue>());
    main_tool->remove_sprite(key, val);
    delete item;    // This will also remove the item from the list
}

void MainWindow::item_changed(QListWidgetItem *curr, QListWidgetItem *prev)
{
    if (curr == nullptr)
        return;
    const sprite::SpriteKey key(curr->data(Qt::UserRole).value<sprite::SpriteKey>());
    idlabel->setText(QString("%1").arg(key.id, 2, 16, QLatin1Char('0')).toUpper());
    eblabel->setText(QString::number(key.extra_bits()));
}

void MainWindow::open_file()
{
    QString name, errors;
    QMessageBox msg;
    QString item_msg;
    int err;

    if (main_tool->is_open()) {
        msg.setText("A ROM is already opened.");
        msg.setInformativeText("Close it first to open a new one!");
        msg.exec();
        return;
    }

    name = QFileDialog::getOpenFileName(this, "Open Image", last_dir,
            "SNES ROM files (*.smc *.sfc)");
    if (name.isEmpty())
        return;
    err = main_tool->open(name, errors);
    if (err == 2) {
        msg.setText("Found errors:\n" + errors);
        msg.exec();
        return;
    }

    // No errors found, setup the widgets and dialogs
    last_dir = name;
    romnamelabel->setText("ROM name: " + QFileInfo(name).fileName());
    const sprite::SpriteMap &spmap = main_tool->sprite_map();
    for (auto it = spmap.begin(); it != spmap.end(); it++)
        add_list_item(it.key(), it.value());
    enable_disable(true);
    add_dialog->init_ext_field();
    add_dialog->init_inromlist();
}

void MainWindow::close_file()
{
    if (!main_tool->is_open()) {
        not_open_message();
        return;
    }

    if (main_tool->unsaved()) {
        switch(display_save_message()) {
        case QMessageBox::Save:
            main_tool->save();
            break;
        case QMessageBox::Cancel:
            return;
        }
    }
    sprite_list->clear();
    main_tool->close();
    enable_disable(false);
    romnamelabel->setText("ROM name:");
    add_dialog->clear_inromlist();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret;

    if (!main_tool->unsaved()) {
        event->accept();
        return;
    }

    ret = display_save_message();
    switch (ret) {
    case QMessageBox::Save:
        main_tool->save();
        event->accept();
        break;
    case QMessageBox::Discard:
        event->accept();
        break;
    case QMessageBox::Cancel:
        event->ignore();
    }
}

