#include "fldialog.h"
#include "ui_funlistdialog.h"
#include "analyse.h"

FunListDialog::FunListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FunListDialog)
{
    ui->setupUi(this);
}

FunListDialog::~FunListDialog()
{
    delete ui;
}

// Заполнение списка названиями функций
void FunListDialog::setup(TResultList* r)
{
    ui->listWidget->clear();
    for (unsigned i = 0; i < r->size(); i++)
        ui->listWidget->addItem(QString((*r)[i].getName().c_str()));
    ui->listWidget->setCurrentRow(0);
}

QString FunListDialog::getFuncName(void)
{
    return ui->listWidget->item(ui->listWidget->currentRow())->text();
}

int FunListDialog::getFuncIndex(void)
{
    return ui->listWidget->currentRow();
}

void FunListDialog::changeLanguage(void)
{
    ui->retranslateUi(this);
}
