#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include "dialogs.h"

class QString;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;
class Tool;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Tool *main_tool;
    QString last_dir;

    QLabel *romnamelabel    = new QLabel("ROM name: ");
    QLabel *idlabel         = new QLabel;
    QLabel *eblabel         = new QLabel;
    QLineEdit *namebox      = new QLineEdit;
    QLineEdit *tipbox       = new QLineEdit;
    QLineEdit *extbox       = new QLineEdit;
    QListWidget *sprite_list = new QListWidget;

    AddSpriteDialog *add_dialog = new AddSpriteDialog(this);

    void create_menu(void);
    void create_buttons(QHBoxLayout *lt);

public:
    explicit MainWindow(Tool *tool, QWidget *parent = nullptr);
    ~MainWindow() { }

public slots:
    void open_file(bool checked);
    void add_new_sprite(bool checked);
    void remove_sprite(bool checked);
    void edit_look(bool checked);
    void item_changed(QListWidgetItem *curr, QListWidgetItem *prev);
};

#endif

