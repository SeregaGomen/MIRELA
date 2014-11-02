#ifndef FLDIALOG_H
#define FLDIALOG_H

#include <QDialog>
#include <QString>

class TResultList;

namespace Ui {
    class FunListDialog;
}


class FunListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FunListDialog(QWidget *parent = 0);
    ~FunListDialog();
    void setup(TResultList*);
    void changeLanguage(void);
    QString getFuncName(void);
    int getFuncIndex(void);
private:
    Ui::FunListDialog *ui;
};

#endif // FLDIALOG_H
