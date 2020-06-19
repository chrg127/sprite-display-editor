#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
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
    QPlainTextEdit *tip;
    QVBoxLayout *mainlt;
    QFormLayout *flt;

    void toggle_extbox(const sprite::SpriteKey *key, const sprite::SpriteValue *val);
public:
    SpriteFormDialog(QWidget *parent = nullptr);
    ~SpriteFormDialog() { }

    QString getname() const
    {
        return name->text();
    }
    QString gettip() const
    {
        return tip->toPlainText();
    }
    QString get_ext() const
    {
        return extbt->text();
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
    unsigned char getid() const
    {
        return id->value();
    }
    unsigned char geteb() const
    {
        return eb->value();
    }
public slots:
    void before_accept();
    void before_reject();
    void sb_values_changed(int newv);
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
    void fill_boxes(const sprite::SpriteKey &sk, const sprite::SpriteValue &sv);
};

#endif
