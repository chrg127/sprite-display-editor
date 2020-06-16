#include "dialogs.h"

#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#ifdef DEBUG
#include <QDebug>
#endif

AddSpriteDialog::AddSpriteDialog(QWidget *parent)
    : QDialog(parent)
{
    QFormLayout *flt        = new QFormLayout;
    QHBoxLayout *hlt        = new QHBoxLayout;
    QVBoxLayout *mainlt     = new QVBoxLayout;
    QPushButton *btadd      = new QPushButton("Add");
    QPushButton *btclose    = new QPushButton("Close");

    setSizeGripEnabled(true);
    setWindowTitle("Add a sprite");
    connect(btadd, &QAbstractButton::clicked, this, &AddSpriteDialog::before_accept);
    connect(btclose, &QAbstractButton::clicked, this, &AddSpriteDialog::reject);

    flt->addRow(new QLabel("ID (00 - FF): "),               id);
    flt->addRow(new QLabel("Extra bits (0 - 3): "),         eb);
    flt->addRow(new QLabel("Name: "),                       name);
    flt->addRow(new QLabel("Tooltip: "),                    tip);
    flt->addRow(new QLabel("Extension Bytes (00 - FF): "),  extbt);
    hlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    hlt->addWidget(btadd);
    hlt->addWidget(btclose);
    hlt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->addLayout(flt);
    mainlt->addLayout(hlt);
    setLayout(mainlt);
}

void AddSpriteDialog::before_accept(bool checked)
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

