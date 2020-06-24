#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>

class QString;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QMenu;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;
class Tool;
class AddSpriteDialog;
class EditSpriteDialog;
class EditDisplayDialog;
namespace sprite {
    class SpriteKey;
    class SpriteTile;
    class SpriteValue;
}

#define MENU_COUNT 3

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
    QMenu *menus[MENU_COUNT];
    AddSpriteDialog *add_dialog;
    EditSpriteDialog *edit_dialog;
    EditDisplayDialog *display_dialog;

    void add_menu_item(QMenu *menu, const QString &text, void (MainWindow::*func)(void),
        bool enable);
    void create_menu();
    void create_labels(QHBoxLayout *lt);
    void create_buttons(QHBoxLayout *lt);
    QListWidgetItem *add_list_item(const sprite::SpriteKey &key, const sprite::SpriteValue &val, int pos = -1);
    QListWidgetItem *find_item(const sprite::SpriteKey &key, const sprite::SpriteValue &val);
    void enable_disable(bool enable);

public:
    explicit MainWindow(Tool *tool, QWidget *parent = nullptr);
    ~MainWindow() { }

public slots:
    void open_file();
    void close_file();
    void add_sprite();
    void edit_sprite();
    void edit_look();
    void remove_sprite();
    void item_changed(QListWidgetItem *curr, QListWidgetItem *prev);
    void closeEvent(QCloseEvent *event);
};

#endif

