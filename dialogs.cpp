#define DEBUG

#include "dialogs.h"

#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFont>
#include <QValidator>
#include "sprite.h"

#ifdef DEBUG
#include <QDebug>
#endif

using sprite::SpriteKey;
using sprite::SpriteValue;

static QChar fixupchar(QChar c)
{
    if (!isxdigit(c.toLatin1()))
        c = '0';
    if (c.isLower())
        c = c.toUpper();
    return c;
}

/* Validator for the extension bytes's textbox */
class HexValidator : public QValidator {
public:
    HexValidator(QObject *parent)
        : QValidator(parent)
    { }

private:
    void fixup(QString &input) const
    {
        for (int i = 0; i < input.size(); i++)
            input[i] = fixupchar(input[i]);
    }

    State validate(QString &input, int &pos) const
    {
        for (int i = 0; i < input.size(); i++) {
            if (!isxdigit(input[i].toLatin1()))
                return Invalid;
            input[i] = fixupchar(input[i]);
        }
        return Acceptable;
    }
};



SpriteForm::SpriteForm(QWidget *parent)
    : QWidget(parent)
{
    QFormLayout *flt = new QFormLayout(this);
    name    = new QLineEdit;
    extbt   = new QLineEdit;
    tip     = new QPlainTextEdit;

    extbt->setValidator(new HexValidator(this));
    tip->setFixedHeight(17*5);
    tip->setTabChangesFocus(true);
    flt->addRow("Name: ",                       name);
    flt->addRow("Tooltip: ",                    tip);
    flt->addRow("Extension Bytes\n(00 - FF): ", extbt);
}

void SpriteForm::toggle_extbox(const SpriteKey *key, const SpriteValue *val)
{
    if (key == nullptr)
        return;

    unsigned char extsize = key->get_ext_size();
    if (extsize == 0) {
        extbt->clear();
        extbt->setEnabled(false);
    } else {
        extbt->setEnabled(true);
        extbt->setMaxLength(extsize*2);
        extbt->home(false);
        if (val != nullptr)
            extbt->setText(val->extb2str(extsize));
    }
}



AddSpriteDialog::AddSpriteDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainlt     = new QVBoxLayout;
    QFormLayout *spinlt     = new QFormLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;
    QHBoxLayout *notinslt   = new QHBoxLayout;
    spform = new SpriteForm;
    QWidget *space = new QWidget;

    setSizeGripEnabled(true);
    setWindowTitle(QStringLiteral("Add sprite"));
    create_spinboxes(spinlt);
    create_buttons(buttonlt);
    create_spoiler(notinslt);
    space->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    mainlt->addLayout(spinlt);
    mainlt->addWidget(spform);
    mainlt->addLayout(buttonlt);
    mainlt->addLayout(notinslt);
    mainlt->addWidget(not_ins_list);

    mainlt->addWidget(space, 1);
    setLayout(mainlt);
}

void AddSpriteDialog::create_spinboxes(QFormLayout *lt)
{
    QLabel *tmp             = new QLabel(QStringLiteral("ID (00 - FF):"));
    QLabel *tmp2            = new QLabel(QStringLiteral("Extra bits (0 - 3):"));
    id = new PaddedSpinBox;
    eb = new PaddedSpinBox;

    id->setRange(0, 0xFF);
    id->setDisplayIntegerBase(16);
    id->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QFont f = id->font();
    f.setCapitalization(QFont::AllUppercase);
    id->setFont(f);
    eb->setRange(0, 3);
    eb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    lt->addRow(tmp, id);
    lt->addRow(tmp2, eb);

    connect(id, QOverload<int>::of(&QSpinBox::valueChanged), [=](int newv) {
                SpriteKey key(newv, eb->value());
                spform->toggle_extbox(&key, nullptr);
            });
    connect(eb, QOverload<int>::of(&QSpinBox::valueChanged), [=](int newv) {
                SpriteKey key(id->value(), newv);
                spform->toggle_extbox(&key, nullptr);
            });
}

void AddSpriteDialog::create_buttons(QHBoxLayout *buttonlt)
{
    QPushButton *add        = new QPushButton(QStringLiteral("Add"));
    QPushButton *close      = new QPushButton(QStringLiteral("Close"));

    buttonlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    buttonlt->addWidget(add);
    buttonlt->addWidget(close);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignRight);

    connect(add, &QAbstractButton::released, this, &AddSpriteDialog::accept);
    connect(close, &QAbstractButton::released, this, &AddSpriteDialog::reject);
}

void AddSpriteDialog::create_spoiler(QHBoxLayout *lt)
{
    QLabel *lb      = new QLabel(QStringLiteral("Not inserted:"));
    not_ins_list    = new QListWidget;
    QPushButton *btn = new QPushButton("Show");

    not_ins_list->hide();
    not_ins_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    lt->addWidget(lb);
    lt->addWidget(btn, 1, Qt::AlignLeft);

    connect(btn, &QAbstractButton::released, [=]() {
                if (not_ins_list->isHidden()) {
                    btn->setText("Hide");
                    not_ins_list->show();
                } else {
                    btn->setText("Show");
                    not_ins_list->hide();
                }
            });
}

void AddSpriteDialog::clear_fields()
{
    id->setValue(0);
    eb->setValue(0);
    spform->name->clear();
    spform->tip->clear();
    spform->extbt->clear();
}

void AddSpriteDialog::init_ext_field()
{
    SpriteKey key(0, 0);
    spform->toggle_extbox(&key, nullptr);
}



EditSpriteDialog::EditSpriteDialog(QWidget *parent)
    : QDialog(parent)
{
    QHBoxLayout *labellt    = new QHBoxLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;
    QVBoxLayout *mainlt     = new QVBoxLayout;
    QPushButton *save       = new QPushButton(QStringLiteral("Save"));
    QPushButton *close      = new QPushButton(QStringLiteral("Close"));
    QWidget *space          = new QWidget;
    QLabel *tmp             = new QLabel(QStringLiteral("ID:"));
    QLabel *tmp2            = new QLabel(QStringLiteral("Extra bits:"));
    id  = new QLabel;
    eb  = new QLabel;
    spform = new SpriteForm;

    setMinimumWidth(400);
    setWindowTitle(QStringLiteral("Edit Sprite"));
    tmp->setAlignment(Qt::AlignRight);
    id->setAlignment(Qt::AlignLeft);
    tmp2->setAlignment(Qt::AlignRight);
    eb->setAlignment(Qt::AlignLeft);
    space->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    labellt->addWidget(tmp);
    labellt->addWidget(id);
    labellt->addWidget(tmp2);
    labellt->addWidget(eb);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    buttonlt->addWidget(save);
    buttonlt->addWidget(close);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->addLayout(labellt);
    mainlt->addWidget(spform);
    mainlt->addLayout(buttonlt);
    mainlt->addWidget(space, 1);
    setLayout(mainlt);

    connect(save, &QAbstractButton::released, [=]() {
                return accept();
            });
    connect(close, &QAbstractButton::released, [=]() {
                return reject();
            });
}

void EditSpriteDialog::init_fields(const SpriteKey &sk, const SpriteValue &sv)
{
    spform->name->setText(sv.name);
    spform->name->home(false);
    spform->tip->setPlainText(sv.tooltip);
    spform->toggle_extbox(&sk, &sv);
    id->setText(QString("%1").arg(sk.id, 2, 16, QLatin1Char('0')).toUpper());
    eb->setText(QString::number(sk.extra_bits()));
}



EditDisplayDialog::EditDisplayDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainlt = new QVBoxLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;
    QPushButton *save       = new QPushButton(QStringLiteral("Save"));
    QPushButton *close      = new QPushButton(QStringLiteral("Close"));
    text = new QLabel(QStringLiteral("not implemented yet, sorry!"));

    setWindowTitle(QStringLiteral("Edit Display Graphics"));
    buttonlt->addWidget(save);
    buttonlt->addWidget(close);
    mainlt->addLayout(buttonlt);
    mainlt->addWidget(text);
    setLayout(mainlt);

    connect(save, &QAbstractButton::released, this, &EditDisplayDialog::accept);
    connect(close, &QAbstractButton::released, this, &EditDisplayDialog::reject);
}

