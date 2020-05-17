#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>
#include "tool.h"
#include "sprite.h"

class QPushButton;
class QLabel;
class QListWidget;
class QLineEdit;
class QMenu;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Tool *tool;

    QWidget *center_widget;
    QMenu *filemenu;
    QMenu *aboutmenu;
    QPushButton *addspritebtn;
    QPushButton *moveupbtn;
    QPushButton *movedownbtn;
    QPushButton *editlookbtn;
    QLabel *namelabel;
    QLabel *cmdlabel;
    QLabel *tiplabel;
    QLabel *romnamelabel;
    QListWidget *spritelistwidget;
    QLineEdit *namebox;
    QLineEdit *cmdbox;
    QLineEdit *tipbox;

    QString last_dir;

    void createactions(void);
    void createmenu(void);
    void createbuttons(void);
    void createlabels(void);
    void createlistwidget(void);
    void createlineedits(void);
    
public:
    explicit MainWindow(QWidget *parent = 0);

public slots:
    void open_file(bool checked);
    void close_file(bool checked);
    void save_file(bool checked);

};

#endif

