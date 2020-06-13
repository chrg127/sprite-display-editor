#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AddSpriteDialog : public QDialog {
    Q_OBJECT
private:
    QLabel *id      = new QLabel("ID (00 - FF): ");
    QLabel *eb      = new QLabel("Extra bits (0 - 3): ");
    QLabel *name    = new QLabel("Name: ");
    QLabel *tip     = new QLabel("Tooltip: ");
    QLabel *extbt   = new QLabel("Extension Bytes (00 - FF): ");
    QLineEdit *idbox    = new QLineEdit;
    QLineEdit *ebbox    = new QLineEdit;
    QLineEdit *namebox  = new QLineEdit;
    QLineEdit *tipbox   = new QLineEdit;
    QLineEdit *extbtbox = new QLineEdit;
    QPushButton *btadd  = new QPushButton("Add");
    QPushButton *btclose    = new QPushButton("Close");
public:
    AddSpriteDialog(QWidget *parent = nullptr);
    ~AddSpriteDialog() { }
public slots:
    void before_accept(bool checked);
};
/*
class EditLookDialog : public QDialog {

};
*/
#endif
