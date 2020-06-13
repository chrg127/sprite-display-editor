#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include "sprite.h"
#include "tool.h"
#include "dialogs.h"

class QString;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Tool *main_tool;
    QString last_dir;

    QWidget *center_widget = new QWidget(this);
    QLabel *romnamelabel   = new QLabel("ROM name: ");
    QLabel *namelabel    = new QLabel("Sprite name: ");
    QLabel *idlabel      = new QLabel("Sprite tooltip: ");
    QLabel *tiplabel     = new QLabel("Sprite tooltip: ");
    QLineEdit *namebox   = new QLineEdit;
    QLineEdit *idbox     = new QLineEdit;
    QLineEdit *tipbox    = new QLineEdit;
    QPushButton *editlookbtn        = new QPushButton("Edit look");;
    QPushButton *addspritebtn       = new QPushButton("Add new sprite");;
    QPushButton *removespritebtn    = new QPushButton("Remove sprite");;
    QListWidget *filled_sprite_list     = new QListWidget;
    QListWidget *unfilled_sprite_list   = new QListWidget;

    AddSpriteDialog *add_dialog = new AddSpriteDialog(this);

    void create_menu(void);
    void create_edit_zone(QFormLayout *flt);
    void create_buttons(QHBoxLayout *lt);
    void create_lists(QHBoxLayout *lt);

public:
    explicit MainWindow(Tool *tool, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void open_file(bool checked);
    void add_new_sprite(bool checked);
    void remove_sprite(bool checked);
    void edit_look(bool checked);
    //void btadd_func(bool checked);
};

#endif

