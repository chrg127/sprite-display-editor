#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QListWidget>
#include "sprite_defines.h"

class QPushButton;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
namespace sprite {
    class SpriteKey;
    class SpriteValue;
}
class Tool;

/* Just a simple class made so I could have padding
 * (fuck QT for not adding padding for spinboxes */
class PaddedSpinBox : public QSpinBox {

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


class SpriteForm : public QWidget {
    Q_OBJECT
public:
    QLineEdit *name, *extbt;
    QPlainTextEdit *tip;

    SpriteForm(QWidget *parent = nullptr);
    ~SpriteForm() { }

    void toggle_extbox(const sprite::SpriteKey *key, const sprite::SpriteValue *val);
};



class AddSpriteDialog : public QDialog {
    Q_OBJECT
private:
    Tool *main_tool;
    SpriteForm *spform;
    PaddedSpinBox *id, *eb;
    QListWidget *inrom_list;

    void create_spinboxes(QFormLayout *lt);
    void create_buttons(QHBoxLayout *lt);
    void create_spoiler(QHBoxLayout *lt);

public:
    AddSpriteDialog(Tool *t, QWidget *parent = nullptr);
    ~AddSpriteDialog() { }

    void init_ext_field();
    void init_inromlist();
    void clear_fields();

    unsigned char getid() const
    {
        return id->value();
    }

    unsigned char geteb() const
    {
        return eb->value();
    }

    QString getname() const
    {
        return spform->name->text();
    }

    QString gettip() const
    {
        return spform->tip->toPlainText();
    }

    QString get_ext() const
    {
        return spform->extbt->text();
    }

    void clear_inromlist()
    {
        inrom_list->clear();
    }
};



class EditSpriteDialog : public QDialog {
    Q_OBJECT

private:
    QLabel *id, *eb;
    SpriteForm *spform;

public:
    EditSpriteDialog(QWidget *parent = nullptr);
    ~EditSpriteDialog() { }
    void init_fields(const sprite::SpriteKey &key, const sprite::SpriteValue &val);

    QString getname() const
    {
        return spform->name->text();
    }

    QString gettip() const
    {
        return spform->tip->toPlainText();
    }

    QString get_ext() const
    {
        return spform->extbt->text();
    }
};



class EditDisplayDialog : public QDialog {
    Q_OBJECT

private:
    QLabel *text;

public:
    EditDisplayDialog(QWidget *parent = nullptr);
    ~EditDisplayDialog() { }
};

#endif

