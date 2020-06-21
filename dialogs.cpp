#define DEBUG

#include "dialogs.h"

#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QSpinBox>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFont>
#include <QValidator>
#include "sprite.h"

#ifdef DEBUG
#include <QDebug>
#endif

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

SpriteFormDialog::SpriteFormDialog(QWidget *parent)
    : QDialog(parent)
{
    name    = new QLineEdit;
    extbt   = new QLineEdit;
    tip     = new QPlainTextEdit;
    mainlt  = new QVBoxLayout;
    flt     = new QFormLayout;

    setSizeGripEnabled(true);

    extbt->setValidator(new HexValidator(this));
    tip->setFixedHeight(17*5);
    tip->setTabChangesFocus(true);

    flt->addRow("Name: ", name);
    flt->addRow("Tooltip: ", tip);
    flt->addRow("Extension Bytes\n(00 - FF): ", extbt);

    mainlt->addLayout(flt);
    setLayout(mainlt);
}

void SpriteFormDialog::toggle_extbox(const sprite::SpriteKey *key, const sprite::SpriteValue *val)
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


/*
*/
AddSpriteDialog::AddSpriteDialog(QWidget *parent)
    : SpriteFormDialog(parent)
{
    QHBoxLayout *spinlt     = new QHBoxLayout;
    QHBoxLayout *buttonlt     = new QHBoxLayout;
    QPushButton *add    = new QPushButton(QStringLiteral("Add"));
    QPushButton *close  = new QPushButton(QStringLiteral("Close"));
    id = new PaddedSpinBox;
    eb = new PaddedSpinBox;

    id->setRange(0, 0xFF);
    id->setDisplayIntegerBase(16);
    QFont f = id->font();
    f.setCapitalization(QFont::AllUppercase);
    id->setFont(f);
    eb->setRange(0, 3);

    setWindowTitle(QStringLiteral("Add sprite"));

    spinlt->addWidget(new QLabel(QStringLiteral("ID (00 - FF):")));
    spinlt->addWidget(id);
    spinlt->addWidget(new QLabel(QStringLiteral("Extra bits (0 - 3):")));
    spinlt->addWidget(eb);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    buttonlt->addWidget(add);
    buttonlt->addWidget(close);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->insertLayout(0, spinlt);
    mainlt->addLayout(buttonlt);

    connect(add, &QAbstractButton::released, this, &AddSpriteDialog::on_accept);
    connect(close, &QAbstractButton::released, this, &AddSpriteDialog::on_reject);
    connect(id, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &AddSpriteDialog::sb_values_changed);
    connect(eb, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &AddSpriteDialog::sb_values_changed);
}

void AddSpriteDialog::on_accept()
{
    id->setValue(0);
    eb->setValue(0);
    name->clear();
    tip->clear();
    extbt->clear();
    return accept();
}

void AddSpriteDialog::on_reject()
{
    id->setValue(0);
    eb->setValue(0);
    name->clear();
    tip->clear();
    extbt->clear();
    return reject();
}

void AddSpriteDialog::sb_values_changed(int newv)
{
    sprite::SpriteKey sk(id->value(), eb->value());
    toggle_extbox(&sk, nullptr);
}



EditSpriteDialog::EditSpriteDialog(QWidget *parent)
    : SpriteFormDialog(parent)
{
    QPushButton *save       = new QPushButton(QStringLiteral("Save"));
    QPushButton *close      = new QPushButton(QStringLiteral("Close"));
    QHBoxLayout *labellt    = new QHBoxLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;
    QLabel *tmp = new QLabel(QStringLiteral("ID:"));
    QLabel *tmp2 = new QLabel(QStringLiteral("Extra bits:"));
    id = new QLabel;
    eb = new QLabel;

    setMinimumWidth(400);
    setWindowTitle(QStringLiteral("Edit Sprite"));

    tmp->setAlignment(Qt::AlignRight);
    tmp2->setAlignment(Qt::AlignRight);

    labellt->addWidget(tmp);
    labellt->addWidget(id);
    labellt->addWidget(tmp2);
    labellt->addWidget(eb);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    buttonlt->addWidget(save);
    buttonlt->addWidget(close);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->insertLayout(0, labellt);
    mainlt->addLayout(buttonlt);

    connect(save, &QAbstractButton::released, this, &EditSpriteDialog::on_accept);
    connect(close, &QAbstractButton::released, this, &EditSpriteDialog::on_reject);
}

void EditSpriteDialog::on_accept()
{
    return accept();
}

void EditSpriteDialog::on_reject()
{
    return reject();
}

void EditSpriteDialog::fill_boxes(const sprite::SpriteKey &sk, const sprite::SpriteValue &sv)
{
    QString extmask;

    name->setText(sv.name);
    name->home(false);
    tip->setPlainText(sv.tooltip);
    toggle_extbox(&sk, &sv);
    id->setText(QString("%1").arg(sk.id, 2, 16, QLatin1Char('0')).toUpper());
    eb->setText(QString::number(sk.extra_bits()));
}

