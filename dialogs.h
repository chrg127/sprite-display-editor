#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLabel>
#include <QLineEdit>

class AddSpriteDialog : public QDialog {
    Q_OBJECT
private:
    QLabel *id      = new QLabel("ID: ");
    QLabel *eb      = new QLabel("Extra bits: ");
    QLabel *name    = new QLabel("Name: ");
    QLabel *tip     = new QLabel("Tooltip: ");
    QLineEdit *idbox    = new QLineEdit;
    QLineEdit *ebbox    = new QLineEdit;
    QLineEdit *namebox  = new QLineEdit;
    QLineEdit *tipbox   = new QLineEdit;
public:
    AddSpriteDialog(QWidget *parent = nullptr);
    ~AddSpriteDialog() { }
};
/*
class EditLookDialog : public QDialog {

};
*/
#endif
