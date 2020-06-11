#include "dialogs.h"

#include <QFormLayout>

AddSpriteDialog::AddSpriteDialog(QWidget *parent)
    : QDialog(parent)
{
    QFormLayout *lt = new QFormLayout;
    lt->addRow(id, idbox);
    lt->addRow(eb, ebbox);
    lt->addRow(name, namebox);
    lt->addRow(tip, tipbox);
    setLayout(lt);
}
