#ifndef _WINDOW_H
#define _WINDOW_H

#include <QWidget>

class QPushButton;
class QLabel;
class QListView;
class QLineEdit;

class Window : public QWidget
{
    //Q_OBJECT

    QPushButton *addsprite;
    QPushButton *moveup;
    QPushButton *movedown;
    QPushButton *editlook;
    QLabel *spritename;
    QLabel *spritecmd;
    QLabel *spritetip;
    QListView *spritelist;
    QLineEdit *namebox;
    QLineEdit *cmdbox;
    QLineEdit *tipbox;

public:
    explicit Window(QWidget *parent = 0);

signals:

public slots:

};

#endif
