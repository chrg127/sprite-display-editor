#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>

class QPushButton;
class QLabel;
class QListView;
class QLineEdit;
class QMenu;

class Window : public QMainWindow {
    Q_OBJECT

private:
    QWidget *center_widget;

    QMenu *filemenu;
    QMenu *aboutmenu;
    QAction *openact;
    QPushButton *addspritebtn;
    QPushButton *moveupbtn;
    QPushButton *movedownbtn;
    QPushButton *editlookbtn;
    QLabel *namelabel;
    QLabel *cmdlabel;
    QLabel *tiplabel;
    QLabel *romnamelabel;
    QListView *spritelistview;
    QLineEdit *namebox;
    QLineEdit *cmdbox;
    QLineEdit *tipbox;

    void createactions(void);
    void createmenu(void);
    void createbuttons(void);
    void createlabels(void);
    void createlistview(void);
    void createlineedits(void);
    
public:
    explicit Window(QWidget *parent = 0);
    void open_file(bool checked);

signals:

public slots:

private slots:

};

#endif
