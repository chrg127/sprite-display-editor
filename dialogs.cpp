#define DEBUG

#include "dialogs.h"

#include <QObject>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QFont>
#include "sprite.h"
#include "hexvalidator.h"

#ifdef DEBUG
#include <QDebug>
#endif

SpriteFormDialog::SpriteFormDialog(QWidget *parent)
    : QDialog(parent)
{
    name    = new QLineEdit;
    extbt   = new QLineEdit;
    tip     = new QTextEdit;
    mainlt  = new QVBoxLayout;
    flt     = new QFormLayout;

    extbt->setValidator(new HexValidator(this));

    flt->addRow("Name: ", name);
    flt->addRow("Tooltip: ", tip);
    flt->addRow("Extension Bytes\n(00 - FF): ", extbt);

    mainlt->addLayout(flt);
    setLayout(mainlt);

    setSizeGripEnabled(true);
}



AddSpriteDialog::AddSpriteDialog(QWidget *parent)
    : SpriteFormDialog(parent)
{
    QHBoxLayout *spinlt     = new QHBoxLayout;
    QHBoxLayout *buttonlt     = new QHBoxLayout;
    QPushButton *add    = new QPushButton(QStringLiteral("Add"));
    QPushButton *close  = new QPushButton(QStringLiteral("Close"));
    id = new QSpinBox;
    eb = new QSpinBox;

    id->setRange(0, 0xFF);
    id->setDisplayIntegerBase(16);
    QFont f = id->font();
    f.setCapitalization(QFont::AllUppercase);
    id->setFont(f);
    eb->setRange(0, 3);

    setWindowTitle(QStringLiteral("Add sprite"));
    connect(add, &QAbstractButton::released, this, &AddSpriteDialog::before_accept);
    connect(close, &QAbstractButton::released, this, &AddSpriteDialog::before_reject);

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
}

void AddSpriteDialog::before_accept()
{
    QMessageBox msg;

    /*
    if (id->displayText() == "" && eb->displayText() == "" &&
        name->displayText() == "" && //tip->displayText() == "" &&
        extbt->displayText() == "")
    {
        msg.setText("Fill out at least one of the boxes first.");
        msg.exec();
        return;
    }*/
    return accept();
}

void AddSpriteDialog::before_reject()
{
    return reject();
}



EditSpriteDialog::EditSpriteDialog(QWidget *parent)
    : SpriteFormDialog(parent)
{
    QPushButton *save       = new QPushButton(QStringLiteral("Save"));
    QPushButton *close      = new QPushButton(QStringLiteral("Close"));
    QHBoxLayout *labellt    = new QHBoxLayout;
    QHBoxLayout *buttonlt   = new QHBoxLayout;

    id = new QLabel;
    eb = new QLabel;

    setMinimumWidth(400);
    setWindowTitle(QStringLiteral("Edit Sprite"));
    connect(save, &QAbstractButton::released, this, &EditSpriteDialog::before_accept);
    connect(close, &QAbstractButton::released, this, &EditSpriteDialog::before_reject);

    labellt->addWidget(new QLabel(QStringLiteral("ID:")));
    labellt->addWidget(id);
    labellt->addWidget(new QLabel(QStringLiteral("Extra bits:")));
    labellt->addWidget(eb);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    buttonlt->addWidget(save);
    buttonlt->addWidget(close);
    buttonlt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->insertLayout(0, labellt);
    mainlt->addLayout(buttonlt);
}

void EditSpriteDialog::before_accept()
{
    return accept();
}

void EditSpriteDialog::before_reject()
{
    name->clear();
    tip->clear();
    extbt->clear();
    return reject();
}

void EditSpriteDialog::fill_boxes(const sprite::SpriteKey &sk, const sprite::SpriteValue sv)
{
    unsigned char extsize = sk.get_ext_size();
    QString extstr, extmask;

    name->setText(sv.name);
    name->home(false);
    tip->setText(sv.tooltip);
    //tip->home(false);
    if (extsize == 0)
        extbt->setEnabled(false);
    else {
        extbt->setEnabled(true);
        sv.extb2str(extstr, extsize);
        extbt->setText(extstr);
        extbt->setInputMask(extmask);
        extbt->home(false);
    }
    id->setText(QString("%1").arg(sk.id));
    eb->setText(QString("%1").arg(sk.extra_bits()));
}

