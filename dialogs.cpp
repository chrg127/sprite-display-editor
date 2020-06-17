#define DEBUG

#include "dialogs.h"

#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "hexvalidator.h"

#ifdef DEBUG
#include <QDebug>
#endif

SpriteFormDialog::SpriteFormDialog(QWidget *parent)
    : QDialog(parent)
{
    QFormLayout *flt = new QFormLayout;
    
    extbt->setValidator(new HexValidator(this));

    flt->addRow(new QLabel("ID (00 - FF): "),               id);
    flt->addRow(new QLabel("Extra bits (0 - 3): "),         eb);
    flt->addRow(new QLabel("Name: "),                       name);
    flt->addRow(new QLabel("Tooltip: "),                    tip);
    flt->addRow(new QLabel("Extension Bytes (00 - FF): "),  extbt);
    mainlt->addLayout(flt);

    setSizeGripEnabled(true);
    setLayout(mainlt);
}

AddSpriteDialog::AddSpriteDialog(QWidget *parent)
    : SpriteFormDialog(parent)
{
    QHBoxLayout *lt       = new QHBoxLayout;
    QPushButton *add      = new QPushButton("Add");
    QPushButton *close    = new QPushButton("Close");

    setWindowTitle("Add sprite");
    connect(add, &QAbstractButton::released, this, &AddSpriteDialog::before_accept);
    connect(close, &QAbstractButton::released, this, &AddSpriteDialog::reject);

    lt->addWidget(new QWidget, 0, Qt::AlignLeft);
    lt->addWidget(add);
    lt->addWidget(close);
    lt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->addLayout(lt);
}

void AddSpriteDialog::before_accept()
{
    QMessageBox msg;

    if (id->displayText() == "" && eb->displayText() == "" &&
        name->displayText() == "" && tip->displayText() == "" &&
        extbt->displayText() == "")
    {
        msg.setText("Fill out at least one of the boxes first.");
        msg.exec();
        return;
    }
    return accept();
}



EditSpriteDialog::EditSpriteDialog(QWidget *parent)
    : SpriteFormDialog(parent)
{
    QPushButton *save = new QPushButton("Save");
    QPushButton *close = new QPushButton("Close");
    QHBoxLayout *lt = new QHBoxLayout;

    setWindowTitle("Edit Sprite");
    connect(save, &QAbstractButton::released, this, &EditSpriteDialog::before_accept);
    connect(close, &QAbstractButton::released, this, &EditSpriteDialog::reject);

    lt->addWidget(new QWidget, 0, Qt::AlignLeft);
    lt->addWidget(save);
    lt->addWidget(close);
    lt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->addLayout(lt);
}

void EditSpriteDialog::before_accept()
{
    return accept();
}

