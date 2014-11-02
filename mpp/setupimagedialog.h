#ifndef SETUPIMAGEDIALOG_H
#define SETUPIMAGEDIALOG_H

#include <QDialog>
#include <fstream>
#include "imageparams.h"

namespace Ui {
    class SetupImageDialog;
}

using namespace std;

class SetupImageDialog : public QDialog
{
    Q_OBJECT

public slots:
    void accept(void);
    void changeBkgColor(void);
    void changeQuality(void);

public:
    explicit SetupImageDialog(QWidget *parent = 0);
    ~SetupImageDialog();
    void setup(bool isFunc = false);
    void clear(void);
    void initParams(void);
    void write(QDataStream& file)
    {
        params.write(file);
    }
    void read(QDataStream& file)
    {
        params.read(file);
    }
    void write(ofstream& file)
    {
        params.write(file);
    }
    void read(ifstream& file)
    {
        params.read(file);
    }
    ImageParams& getImageParams(void)
    {
        return params;
    }
    void setImageParams(ImageParams& r, bool isFunc = false)
    {
        params = r;
        setup(isFunc);
    }
    void changeLanguage(void);
private slots:
    void slotChangeKoff(int);
private:
    QColor bkgColor;
    Ui::SetupImageDialog *ui;
    ImageParams params;
    bool reload(void);
    void setKoffValue(void);
};

#endif // SETUPIMAGEDIALOG_H
