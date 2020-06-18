#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>
#include "dialogs.h"

class QString;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;
class Tool;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Tool *main_tool;
    QString last_dir;

    QLabel *romnamelabel;
    QLabel *idlabel;
    QLabel *eblabel;
    QListWidget *sprite_list;
    QPushButton *editsprite;
    QPushButton *editlook;
    QPushButton *removesprite;
    QPushButton *addspritebtn;
    AddSpriteDialog *add_dialog = new AddSpriteDialog(this);
    EditSpriteDialog *edit_dialog = new EditSpriteDialog(this);

    void create_menu();
    void create_labels(QHBoxLayout *lt);
    void create_buttons(QHBoxLayout *lt);

public:
    explicit MainWindow(Tool *tool, QWidget *parent = nullptr);
    ~MainWindow() { }

public slots:
    void open_file();
    void close_file();
    void add_new_sprite();
    void edit_sprite();
    void edit_look();
    void remove_sprite();
    void item_changed(QListWidgetItem *curr, QListWidgetItem *prev);
    void closeEvent(QCloseEvent *event);
};

#endif

