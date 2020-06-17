#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>

class SpriteFormDialog : public QDialog {
    Q_OBJECT
protected:
    QLineEdit *id           = new QLineEdit;
    QLineEdit *eb           = new QLineEdit;
    QLineEdit *name         = new QLineEdit;
    QLineEdit *tip          = new QLineEdit;
    QLineEdit *extbt        = new QLineEdit;
    QVBoxLayout *mainlt     = new QVBoxLayout;
public:
    SpriteFormDialog(QWidget *parent = nullptr);
    ~SpriteFormDialog() { }
public slots:
    virtual void before_accept() = 0;
};

class AddSpriteDialog : public SpriteFormDialog {
    Q_OBJECT
public:
    AddSpriteDialog(QWidget *parent = nullptr);
    ~AddSpriteDialog() { }
public slots:
    void before_accept();
};

class EditSpriteDialog : public SpriteFormDialog {
    Q_OBJECT
public:
    EditSpriteDialog(QWidget *parent = nullptr);
    ~EditSpriteDialog() { }
public slots:
    void before_accept();
};

#endif
