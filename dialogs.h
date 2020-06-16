#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLineEdit>

class AddSpriteDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit *id           = new QLineEdit;
    QLineEdit *eb           = new QLineEdit;
    QLineEdit *name         = new QLineEdit;
    QLineEdit *tip          = new QLineEdit;
    QLineEdit *extbt        = new QLineEdit;
public:
    AddSpriteDialog(QWidget *parent = nullptr);
    ~AddSpriteDialog() { }
public slots:
    void before_accept(bool checked);
};

#endif
