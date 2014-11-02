#include <QColorDialog>
#include <math.h>

#include "setupimagedialog.h"
#include "ui_setupimagedialog.h"

SetupImageDialog::SetupImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupImageDialog)
{
    ui->setupUi(this);
    initParams();

    connect(ui->hsKoff, SIGNAL(valueChanged  (int)),this, SLOT(slotChangeKoff(int)));
    connect(ui->pbColor, SIGNAL(clicked()), this, SLOT(changeBkgColor()));

}

SetupImageDialog::~SetupImageDialog()
{
    delete ui;
}

void SetupImageDialog::changeBkgColor(void)
{
    QColorDialog* cDlg = new QColorDialog(bkgColor,this);

    if (cDlg->exec() == QDialog::Accepted)
    {
        bkgColor = cDlg->currentColor();
//        ui->pbColor->setStyleSheet(QString("background-color: rgb(%1, %2, %3); color: rgb(99, 99, 0)").arg(bkgColor.red()).arg(bkgColor.green()).arg(bkgColor.blue()));
        ui->pbColor->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(bkgColor.red()).arg(bkgColor.green()).arg(bkgColor.blue()));
    }
    delete cDlg;
}

void SetupImageDialog::setup(bool isFunc)
{
    // Инициализация параметров визуализации
    ui->chbMesh->setChecked(params.isMesh);
    ui->chbVertex->setChecked(params.isVertex);
    ui->chbFace->setChecked(params.isFace);
    ui->chbLight->setChecked(params.isLight);
    ui->chbQuality->setChecked(params.isQuality);
    ui->chbCoord->setChecked(params.isCoord);
    ui->chbNegative->setChecked(params.isNegative);
    ui->chbValueScale->setChecked(params.isValueScale);
    ui->chbLimit->setChecked(params.isLimit);
    ui->chbForce->setChecked(params.isForce);

    ui->rbBW->setChecked(params.isBW);
    ui->rbColor->setChecked(params.isRGB);
    ui->rbSpectral->setChecked(params.isSpectral);

    if (isFunc)
        setKoffValue();

    ui->hsAlpha->setSliderPosition(int(params.alpha*10.0));

    switch (params.numColor)
    {
        case 16:
            ui->hsNumColor->setSliderPosition(1);
            break;
        case 128:
            ui->hsNumColor->setSliderPosition(2);
            break;
        case 256:
            ui->hsNumColor->setSliderPosition(3);
            break;
    }


    ui->gbFunc->setEnabled(isFunc);
    ui->gbKoff->setEnabled(isFunc);
    ui->chbValueScale->setEnabled(isFunc);
    ui->chbLimit->setEnabled(!isFunc);
    ui->chbForce->setEnabled(!isFunc);

    // Цвет фона
    bkgColor = params.bkgColor;
    ui->pbColor->setAutoFillBackground(true);
    ui->pbColor->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(params.bkgColor.red()).arg(params.bkgColor.green()).arg(params.bkgColor.blue()));


}

void SetupImageDialog::initParams(void)
{
    params.init();
}

void SetupImageDialog::clear(void)
{
    initParams();
    setup();
}


bool SetupImageDialog::reload(void)
{
    params.isVertex = ui->chbVertex->isChecked();
    params.isMesh = ui->chbMesh->isChecked();
    params.isFace = ui->chbFace->isChecked();
    params.isCoord = ui->chbCoord->isChecked();
    params.isLight = ui->chbLight->isChecked();
    params.isQuality = ui->chbQuality->isChecked();
    params.isRGB = ui->rbColor->isChecked();
    params.isSpectral = ui->rbSpectral->isChecked();
    params.isBW = ui->rbBW->isChecked();
    params.isNegative = ui->chbNegative->isChecked();
    params.isValueScale = ui->chbValueScale->isChecked();
    params.isLimit = ui->chbLimit->isChecked();
    params.isForce = ui->chbForce->isChecked();
    params.koff = ui->leKoff->text().toDouble();
    params.alpha = double(ui->hsAlpha->sliderPosition())*0.1;
    params.bkgColor = bkgColor;

    switch (ui->hsNumColor->sliderPosition())
    {
        case 1:
            params.numColor = 16;
            break;
        case 2:
            params.numColor = 128;
            break;
        case 3:
            params.numColor = 256;
            break;
    }
    return true;
}

void SetupImageDialog::accept(void)
{
    if (reload())
        QDialog::accept();
}

void SetupImageDialog::slotChangeKoff(int pos)
{
    double koff;

    if (pos < 6)
        koff = 1.0/pow(10, 6 - pos);
    else if (pos >  6)
        koff = pow(10, pos)/1.0E+6;
    else
        koff = 0;

    ui->leKoff->setText(tr("%1").arg(koff));
}

void SetupImageDialog::setKoffValue(void)
{
    if (params.koff == 0)
        ui->hsKoff->setValue(6);
    else if (params.koff <= 0.000001)
        ui->hsKoff->setValue(0);
    else if (params.koff <= 0.00001 && params.koff > 0.000001)
        ui->hsKoff->setValue(1);
    else if (params.koff <= 0.0001 && params.koff > 0.00001)
        ui->hsKoff->setValue(2);
    else if (params.koff <= 0.001 && params.koff > 0.0001)
        ui->hsKoff->setValue(3);
    else if (params.koff <= 0.01 && params.koff > 0.001)
        ui->hsKoff->setValue(4);
    else if (params.koff <= 0.1 && params.koff > 0.01)
        ui->hsKoff->setValue(5);
    else if (params.koff > 0 && params.koff <= 10)
        ui->hsKoff->setValue(7);
    else if (params.koff > 10 && params.koff <= 100)
        ui->hsKoff->setValue(8);
    else if (params.koff > 100 && params.koff <= 1000)
        ui->hsKoff->setValue(9);
    else if (params.koff > 1000 && params.koff <= 10000)
        ui->hsKoff->setValue(10);
    else if (params.koff > 10000 && params.koff <= 100000)
        ui->hsKoff->setValue(11);
    else if (params.koff > 100000)
        ui->hsKoff->setValue(12);

    ui->leKoff->setText(QString("%1").arg(params.koff));
}

void SetupImageDialog::changeLanguage(void)
{
    ui->retranslateUi(this);
}
