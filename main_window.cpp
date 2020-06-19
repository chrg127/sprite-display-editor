//#define DEBUG

#include "main_window.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QMenuBar>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QVariant>
#include <QCloseEvent>
#include "dialogs.h"
#include "sprite.h"
#include "tool.h"

#ifdef DEBUG
#include <QDebug>
#endif

#define MIN_WIDTH 400

MainWindow::MainWindow(Tool *tool, QWidget *parent)
    : QMainWindow(parent), main_tool(tool), last_dir(".")//QDir::homePath())
{
    setMinimumWidth(MIN_WIDTH);
    setWindowTitle(QStringLiteral("Sprite Display Editor"));

    QWidget *center_widget  = new QWidget(this);
    QHBoxLayout *labellt    = new QHBoxLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;
    QVBoxLayout *mainlt     = new QVBoxLayout(center_widget);
    sprite_list             = new QListWidget;
    add_dialog              = new AddSpriteDialog(this);
    edit_dialog             = new EditSpriteDialog(this);    

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
}

void MainWindow::create_menu()
{
    QMenu *menu;
    QAction *act;

    menu = menuBar()->addMenu("&File");
    act = new QAction("&Open ROM", this);
    connect(act, &QAction::triggered, this, &MainWindow::open_file);
    menu->addAction(act);
    act = new QAction("&Close ROM", this);
    connect(act, &QAction::triggered, this, &MainWindow::close_file);
    menu->addAction(act);
    menu = menuBar()->addMenu("&About");
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
    editlook->setToolTip(QStringLiteral("Edit the graphics used by the selected sprite."));
    removesprite->setToolTip(QStringLiteral("Removes the selected sprite."));

    addspritebtn->setEnabled(false);
    editsprite->setEnabled(false);
    editlook->setEnabled(false);
    removesprite->setEnabled(false);

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





void MainWindow::add_list_item(const sprite::SpriteKey &key, const sprite::SpriteValue &val,
        int pos)
{
    QString item_msg;
    QListWidgetItem *item;

    if (!val.name.isEmpty())
        item_msg = val.name;
    else
        item_msg = QString("ID: %1; Extra bits: %2").arg(key.id, 0, 16)
            .arg(key.extra_bits(), 0, 16);
    item = new QListWidgetItem(item_msg);
    item->setData(Qt::UserRole, QVariant::fromValue(key));
    item->setData(Qt::UserRole+1, QVariant::fromValue(val));

    if (pos == -1)
        sprite_list->addItem(item);
    else
        sprite_list->insertItem(pos, item);
        
}


void MainWindow::open_file()
{
    QString name, errors;
    QMessageBox msg;
    int err;
    QString item_msg;
    QListWidgetItem *item;

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

    // No errors found, setup the widgets
    last_dir = name;
    romnamelabel->setText("ROM name: " + QFileInfo(name).fileName());
    const sprite::SpriteMap &spmap = main_tool->sprite_map();
    for (auto it = spmap.begin(); it != spmap.end(); it++) {
        add_list_item(it.key(), it.value());
    }

    addspritebtn->setEnabled(true);
    editsprite->setEnabled(true);
    editlook->setEnabled(true);
    removesprite->setEnabled(true);
}

void MainWindow::add_sprite()
{
    if (add_dialog->exec() == 0)
        return; // no sprite to add

    sprite::SpriteKey key(add_dialog->getid(), add_dialog->geteb());
    sprite::SpriteValue val;
    val.name = add_dialog->getname();
    val.tooltip = add_dialog->gettip();
    val.str2extb(add_dialog->get_ext(), key.get_ext_size());
    main_tool->insert_sprite(key, val);
    add_list_item(key, val);

}

void MainWindow::edit_sprite()
{
    QListWidgetItem *item = sprite_list->currentItem();
    QMessageBox msg;
    QString tmp;

    if (item == nullptr) {
        msg.setText(QStringLiteral("No sprites selected. Select a sprite first to use this."));
        msg.exec();
        return;
    }

    // Fill boxes from the list widget item's respective sprite
    sprite::SpriteKey sk(item->data(Qt::UserRole).value<sprite::SpriteKey>());
    sprite::SpriteValue sv(item->data(Qt::UserRole+1).value<sprite::SpriteValue>());
    edit_dialog->fill_boxes(sk, sv);
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
}

void MainWindow::remove_sprite()
{
}

void MainWindow::item_changed(QListWidgetItem *curr, QListWidgetItem *prev)
{
    if (curr == nullptr)
        return;
    const sprite::SpriteKey key(curr->data(Qt::UserRole).value<sprite::SpriteKey>());
    idlabel->setText(QString("%1").arg(key.id, 2, 16, QLatin1Char('0')).toUpper());
    eblabel->setText(QString::number(key.extra_bits()));
}

static int display_save_message()
{
    QMessageBox msg;
    msg.setText(QStringLiteral("There are unsaved changes."));
    msg.setInformativeText(QStringLiteral("Do you want to save your changes?"));
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Save);
    return msg.exec();
}

void MainWindow::close_file()
{
/*    if (!main_tool->is_open()) {
        QMessageBox msg;
        msg.setText(QStringLiteral("No ROM opened. Open a ROM first!"));
        msg.exec();
        return;
    }*/

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
    addspritebtn->setEnabled(false);
    editsprite->setEnabled(false);
    editlook->setEnabled(false);
    removesprite->setEnabled(false);
    romnamelabel->setText("ROM name:");
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

