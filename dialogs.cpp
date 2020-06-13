#define DEBUG

#include "dialogs.h"

#include <QObject>
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
    setSizeGripEnabled(true);
    setWindowTitle("Add a sprite");

    QFormLayout *flt = new QFormLayout;
    QHBoxLayout *hlt = new QHBoxLayout;
    QVBoxLayout *mainlt = new QVBoxLayout;
    flt->addRow(id, idbox);
    flt->addRow(eb, ebbox);
    flt->addRow(name, namebox);
    flt->addRow(tip, tipbox);
    flt->addRow(extbt, extbtbox);
    hlt->addWidget(new QWidget, 0, Qt::AlignLeft);
    hlt->addWidget(btadd);
    hlt->addWidget(btclose);
    hlt->addWidget(new QWidget, 0, Qt::AlignRight);
    mainlt->addLayout(flt);
    mainlt->addLayout(hlt);
    setLayout(mainlt);

    connect(btadd, &QAbstractButton::clicked, this, &AddSpriteDialog::before_accept);
    connect(btclose, &QAbstractButton::clicked, this, &AddSpriteDialog::reject);
}

void AddSpriteDialog::before_accept(bool checked)
{
    QMessageBox msg;

    if (idbox->displayText() == "" && ebbox->displayText() == "" &&
        namebox->displayText() == "" && tipbox->displayText() == "" &&
        extbtbox->displayText() == "")
    {
        msg.setText("Fill out at least one of the boxes first.");
        msg.exec();
        return;
    }
    return accept();
}

/*
void AddSpriteDialog::connect_btadd(const QObject *obj, void (MainWindow::*method)(bool))
{
    //connect(btadd, &QAbstractButton::clicked, obj, method);
}*/
