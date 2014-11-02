#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "matrix.h"

namespace Ui {
class MainWindow;
}

const int maxRecentFiles = 5;


class QTabWidget;
class QTextStream;
class QProgressBar;
class TResultList;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openDocument(void);
    void closeDocument(void)
    {
        closeTab(0);
    }
    void closeTab(int);
    void analyseFunction(void);
    void changeTab(int);
    void setRotate(void);
    void setScale(void);
    void setTranslate(void);
    void setupImageParams(void);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void openRecentFile(void);

private:
    Ui::MainWindow *ui;
    QTabWidget* tabWidget;
    QProgressBar* pb;
    TResultList* rList;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<int> prizn;
    matrix<int> fe;
    matrix<int> surface;
    QMenu* recentFilesMenu;
    QAction* recentFileActs[maxRecentFiles];
    QAction* separatorAct;
    QStringList files;
    int m1;
    int m2;
    int m3;
    bool isDocOpened;
    void readFile(QTextStream&);
    void printInfo(void);
    void checkMenuState(void);
    void createFE(void);
    void createSurface(void);
    void readSettings(void);
    void createRecentMenu(void);
    void writeSettings(void);
    void setupRecentActions(void);
    void updateRecentFileActions(const QString&);
    void loadFile(QString&);
};

#endif // MAINWINDOW_H
