#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
//#include "sprite.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
namespace sprite {
    class SpriteKey;
    class SpriteValue;
}

/* Just a simple class made so I could have padding
 * (fuck QT for not adding padding for spinboxes */
class PaddedSpinBox : public QSpinBox
{
public:
    PaddedSpinBox(QWidget *parent = 0) : QSpinBox(parent)
    { }
protected:
    QString textFromValue(int value) const override
    {
        int width = QString::number(maximum(), displayIntegerBase()).size();
        return QString("%1").arg(value, width, displayIntegerBase(), QChar('0')).toUpper();
    }
};

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
    virtual void on_accept() = 0;
    virtual void on_reject() = 0;
};



class AddSpriteDialog : public SpriteFormDialog {
    Q_OBJECT
private:
    PaddedSpinBox *id, *eb;
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
    void on_accept();
    void on_reject();
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
    void on_accept();
    void on_reject();
    void fill_boxes(const sprite::SpriteKey &sk, const sprite::SpriteValue &sv);
};

#endif
