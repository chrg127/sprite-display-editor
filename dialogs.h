#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "sprite.h"

class QLabel;
class QSpinBox;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;

class SpriteFormDialog : public QDialog {
    Q_OBJECT
protected:
    QLineEdit *name, *extbt;
    QTextEdit *tip;
    QVBoxLayout *mainlt;
    QFormLayout *flt;
public:
    SpriteFormDialog(QWidget *parent = nullptr);
    ~SpriteFormDialog() { }

    void getname(QString &s) const
    {
        s = name->text();
    }
    void gettip(QString &s) const
    {
        s = tip->toPlainText();
    }
    void get_ext(QString &s) const
    {
        s = extbt->text();
    }
public slots:
    virtual void before_accept() = 0;
    virtual void before_reject() = 0;
};



class AddSpriteDialog : public SpriteFormDialog {
    Q_OBJECT
private:
    QSpinBox *id, *eb;
public:
    AddSpriteDialog(QWidget *parent = nullptr);
    ~AddSpriteDialog() { }
public slots:
    void before_accept();
    void before_reject();
};



class EditSpriteDialog : public SpriteFormDialog {
    Q_OBJECT
private:
    QLabel *id, *eb;
public:
    EditSpriteDialog(QWidget *parent = nullptr);
    ~EditSpriteDialog() { }
public slots:
    void before_accept();
    void before_reject();
    void fill_boxes(const sprite::SpriteKey &sk, const sprite::SpriteValue sv);
};

#endif
