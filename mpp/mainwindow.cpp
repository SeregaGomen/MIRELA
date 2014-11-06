#include <QFileDialog>
#include <QProgressBar>
#include <QSettings>
#include <QCloseEvent>

#if QT_VERSION < 5
    #include <QtWebKit/QWebView>
#else
    #include <QWebView>
#endif
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "analyse.h"
#include "fldialog.h"
#include "glfun.h"
#include "setupimagedialog.h"

using namespace std;

//-------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTextCodec* codec =  QTextCodec::codecForName("UTF-8");

    ui->setupUi(this);
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
#endif
    QTextCodec::setCodecForLocale(codec);


    setupRecentActions();
    readSettings();
    createRecentMenu();

    ui->actionStop->setVisible(false);

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);

    setCentralWidget(tabWidget);

    rList = new TResultList;
    isStarted = isStoped = isDocOpened = false;

    m1 = m2 = m3 = 0;

    pb = new QProgressBar(statusBar());
    pb->setTextVisible(true);
    statusBar()->addPermanentWidget(pb);
    pb->hide();

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
    checkMenuState();

}
//-------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
    delete pb;
    delete tabWidget;
    delete rList;
}
//-------------------------------------------------------------------------------------
void MainWindow::openDocument(void)
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Открытие файлов результатов"),windowFilePath(),tr("Файлы результатов (*.txt)"));

    if (fileName.isEmpty())
        return;

    loadFile(fileName);
}
//-------------------------------------------------------------------------------------
void MainWindow::readFile(QTextStream& in)
{
    QString str;
    QTextStream ts(&str);
    int size,
        ti,
        iter = -1;
    double td;
    QString nu,
            u[3],
            sg[6],
            prefix;
    vector<double> u1,
                   u2,
                   u3,
                   sg11,
                   sg12,
                   sg13,
                   sg22,
                   sg23,
                   sg33;

    // Чтение количества узлов в конструкции
    str = in.readLine();
    if (str.left(14).trimmed() != "M1 x M2 x M3")
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Неверный формат файла!"));
        return;
    }
    str = str.right(str.length() - 14);
    str.replace("x"," ");
    ts.setString(&str);
    ts >> m1 >> m2 >> m3;
    if (ts.status() != QTextStream::Ok)
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Неверный формат файла!"));
        return;
    }

    size = m1*m2*m3;
    x.resize(size);
    y.resize(size);
    z.resize(size);
    prizn.resize(size);

    // Пропускаем лишние строки
    while (!in.atEnd())
    {
        str = in.readLine();
        if (!str.contains("NU"))
            continue;
        in.readLine();
        break;
    }


    // Считываем координаты
    pb->setMinimum(0);
    pb->setMaximum(size - 1);
    pb->setFormat(tr("Загрузка координат"));
    for (int i = 0; i < size; i++)
    {
        if (isStoped)
            return;
        if (i%10 == 0)
            pb->setValue(i);

        qApp->processEvents();
        str = in.readLine();
        str.replace("D","E");
        str.replace(".","0.");
        ts.setString(&str);

        ts >> ti >> x[i] >> y[i] >> z[i] >> td >> td >> td >> td >> prizn[i] >> ti;

        if (ts.status() != QTextStream::Ok)
        {
            QMessageBox::information(this, tr("Ошибка"), tr("Неверный формат файла!"));
            return;
        }
    }
    rList->clear();
    // Считываем данные (с учетом итераций)
    while (!in.atEnd())
    {
        str = in.readLine();
        // Поиск номера итерации
        if (str.contains("ITER="))
        {
            str.replace("ITER="," ");
            ts.setString(&str);
            ts >> iter;
            if (ts.status() != QTextStream::Ok)
            {
                QMessageBox::information(this, tr("Ошибка"), tr("Неверный формат файла!"));
                return;
            }
            continue;
        }

        // Пропуск заголовка
        if (!str.contains("NU"))
            continue;
        if (str.contains("NU"))
        {
            // Считываем названия функций
            ts.setString(&str);
            ts >> nu >> u[0] >> u[1] >> u[2] >> sg[0] >> sg[1] >> sg[2] >> sg[3] >> sg[4] >> sg[5];
            if (ts.status() != QTextStream::Ok)
            {
                QMessageBox::information(this, tr("Ошибка"), tr("Неверный формат файла!"));
                return;
            }
            str = in.readLine();
        }
        u1.resize(size);
        u2.resize(size);
        u3.resize(size);
        sg11.resize(size);
        sg12.resize(size);
        sg13.resize(size);
        sg22.resize(size);
        sg23.resize(size);
        sg33.resize(size);
        // Считываем значения функций
        pb->setMinimum(0);
        pb->setMaximum(size - 1);
        pb->setFormat(tr("Загрузка функций"));
        for (int i = 0; i < size; i++)
        {
            if (isStoped)
                return;
            if (i%10 == 0)
                pb->setValue(i);
            qApp->processEvents();
            str = in.readLine();
            if (!str.trimmed().length())
            {
                // Игнорируем пустые строки
                i--;
                continue;
            }
            str.replace("D","E");
            str.replace(".","0.");
            ts.setString(&str);

            ts >> ti >> u1[i] >> u2[i] >> u3[i] >> sg11[i] >> sg12[i] >> sg13[i] >> sg22[i] >> sg23[i] >> sg33[i];

            if (ts.status() != QTextStream::Ok)
            {
                QMessageBox::information(this, tr("Ошибка"), tr("Неверный формат файла!"));
                return;
            }
        }
        // Добавляем функции в список для анализа
        prefix = (iter == -1) ? "" : QString("iter%1->").arg(iter);
        rList->addResult(u1,(prefix + u[0]).toStdString());
        rList->addResult(u2,(prefix + u[1]).toStdString());
        rList->addResult(u3,(prefix + u[2]).toStdString());
        rList->addResult(sg11,(prefix + sg[0]).toStdString());
        rList->addResult(sg12,(prefix + sg[1]).toStdString());
        rList->addResult(sg13,(prefix + sg[2]).toStdString());
        rList->addResult(sg22,(prefix + sg[3]).toStdString());
        rList->addResult(sg23,(prefix + sg[4]).toStdString());
        rList->addResult(sg33,(prefix + sg[5]).toStdString());
    }
}
//-------------------------------------------------------------------------------------
// Вывод сводки о загруженных данных
void MainWindow::printInfo(void)
{
    QString webOut;

    webOut = "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
    // Вывод рез-тов по каждой функции
    webOut += "<h1>";
    webOut += QString(tr("Результаты расчета"));
    webOut += "</h1><br>";
    webOut += QString(tr("Количество узлов в модели (%1x%2x%3): <b>%4</b>\n")).arg(m1).arg(m2).arg(m3).arg(x.size());
    webOut += QString("<br><table border=\"1\" cellpadding=\"4\" cellspacing=\"0\"><tr><th>%1</th><th>%2</th><th>%3</th></tr>").arg(tr("Функция")).arg("min").arg("max");
    for (unsigned i = 0; i < rList->size(); i++)
        webOut += QString("<tr><td>%1</td><td align=\"right\">%2</td><td align=\"right\">%3</td></tr>").arg((*rList)[i].getName().c_str()).arg((*rList)[i].min()).arg((*rList)[i].max());
    webOut += "</table><br>";

    if (!tabWidget->count())
        tabWidget->addTab(new QWebView(),tr("Результаты"));
    qobject_cast<QWebView*>(tabWidget->widget(0))->setHtml(webOut);
    qApp->processEvents();
}
//-------------------------------------------------------------------------------------
void MainWindow::closeTab(int nTab)
{
    int i;

    if (nTab != 0)
    {
//        delete tabWidget->widget(nTab);
        tabWidget->removeTab(nTab);
    }
    else
    {
        i = tabWidget->count() - 1;
        while (tabWidget->count() > 0)
        {
            delete tabWidget->widget(i);
            tabWidget->removeTab(i--);
        }
        setWindowTitle(tr("Постпроцессор системы МІРЕЛА+, ЗНУ, 2014"));
        isDocOpened = false;
        checkMenuState();
    }

}
//-------------------------------------------------------------------------------------
void MainWindow::changeTab(int nTab)
{
    bool isEnabled = (qobject_cast<GLWidget*>(tabWidget->widget(nTab)) == NULL) ? false : true;

    ui->actionRotate->setEnabled(isEnabled);
    ui->actionScale->setEnabled(isEnabled);
    ui->actionTranslate->setEnabled(isEnabled);
//    ui->actionAnalyse->setEnabled(isEnabled);
    ui->actionAnalyse->setEnabled(isDocOpened);
    ui->actionSetup->setEnabled(isEnabled);
    if (isEnabled && isDocOpened)
    {
        if (qobject_cast<GLWidget*>(tabWidget->widget(nTab))->getRotate())
            setRotate();
        else if (qobject_cast<GLWidget*>(tabWidget->widget(nTab))->getScale())
            setScale();
        else
            setTranslate();
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::checkMenuState(void)
{
    bool isEnabled = (qobject_cast<GLWidget*>(tabWidget->currentWidget()) == NULL) ? false : true;

    ui->actionClose->setEnabled(isDocOpened);
    ui->actionAnalyse->setEnabled(isDocOpened);

    ui->actionRotate->setEnabled(isDocOpened && isEnabled);
    ui->actionScale->setEnabled(isDocOpened && isEnabled);
    ui->actionTranslate->setEnabled(isDocOpened && isEnabled);
    ui->actionSetup->setEnabled(isDocOpened && isEnabled);
//    ui->actionRestore->setEnabled(!isUntitled);
//    ui->actionObjectParameters->setEnabled(!isUntitled);
//    ui->actionCalculate->setEnabled(!isUntitled && !femObject.isStarted());
//    ui->actionAnalyse->setEnabled(!isUntitled && femObject.isCalculated());
//    ui->actionAddExpression->setEnabled(!isUntitled && femObject.isCalculated());
//    ui->actionGabarite->setEnabled(!isUntitled);
}
//-------------------------------------------------------------------------------------
void MainWindow::createFE(void)
{
    matrix<int> tmp;
    vector<int> testFE(x.size());
    int nf,
        ne,
        index1 = 0,
        index2 = 0,
        numFE = 0,
        numBadFE = 0,
        count = 0;

    // Реконструируем КЭ
    tmp.resize(x.size(),8);

    pb->setMinimum(0);
    pb->setMaximum(m1*m2*m3 - 1);
    pb->setFormat(tr("Формирование КЭ"));
    for (int n3 = 1; n3 < m3; n3++)
        for (int n2 = 1; n2 < m2; n2++)
            for (int n1 = 1; n1 < m1; n1++)
            {
                nf = n1 + m1*(n2 - 1) + m1*m2*(n3 - 1);

                if (isStoped)
                    return;
                if (count%10 == 0)
                    pb->setValue(count++);
                qApp->processEvents();
                for (int k3 = 1; k3 < 3; k3++)
                    for (int k2 = 1; k2 < 3; k2++)
                        for (int k1 = 1; k1 < 3; k1++)
                        {
                            ne = nf + (k1 - 1) + m1*(k2 - 1) + m1*m2*(k3 - 1);

                            if (prizn[ne - 1] < 0)
                                testFE[index1] = 1;
                            tmp[index1][index2++] = ne - 1;
                            if (index2 == 8)
                            {
                                index1++;
                                index2 = 0;
                            }
                        }
            }
    numFE = index1;


    // Удаляем КЭ с отрицателным NF
    for (int i = 0; i < numFE; i++)
        if (testFE[i])
            numBadFE++;
    if (numBadFE)
    {
        matrix<int> tFE(numFE - numBadFE,8);
        int noFE = 0;

        for (int i = 0; i < numFE; i++)
            if (!testFE[i])
            {
                for (int j = 0; j < 8; j++)
                    tFE[noFE][j] = tmp[i][j];
                noFE++;
            }
        fe = tFE;
        numFE -= numBadFE;
    }
    else
    {
        fe.resize(numFE,8);
        for (int i = 0; i < numFE; i++)
            for (int j = 0; j < 8; j++)
                fe[i][j] = tmp[i][j];
    }
}
//-------------------------------------------------------------------------------------
bool test(int* a,int* b)
{
    bool result;

    for (int i = 0; i < 4; i++)
    {
        result = false;
        for (int j = 0; j < 4; j++)
            if (b[j] == a[i])
            {
                result = true;
                break;
            }
        if (result == false)
            return false;
    }
    return true;
}
//-------------------------------------------------------------------------------------
void MainWindow::createSurface(void)
{
    int cData[6][5] = {
                        {0,4,5,1,7},
                        {6,2,3,7,0},
                        {4,6,7,5,0},
                        {2,0,1,3,5},
                        {1,5,7,3,4},
                        {6,4,0,2,1}
                      },
        data[6],
        l,
        num1 = 6,
        num2 = 4,
        k,
        i,
        j,
        p[6],
        pp[6],
        numFaces = 0;
    double  cx,
            cy,
            cz,
            x1,
            y1,
            z1,
            x2,
            y2,
            z2,
            x3,
            y3,
            z3;
    matrix<int> boundList(6*fe.size1(),num1 + 1);

    pb->setMinimum(0);
    pb->setMaximum(fe.size1() - 1);
    pb->setFormat(tr("Формирование поверхности"));
    for (i = 0; i < (int)fe.size1() - 1; i++)
    {
        if (isStoped)
            return;
        if (i%10 == 0)
            pb->setValue(i);
        qApp->processEvents();
        for (int j = 0; j < num1; j++)
            if (!boundList[i][j])
            {
                for (l = 0; l < num2; l++)
                    p[l]  = fe[i][cData[j][l]];
                for (k = i + 1; k < (int)fe.size1(); k++)
                    for (int m = 0; m < num1; m++)
                        if (!boundList[k][m])
                        {
                            for (int l = 0; l < num2; l++)
                                pp[l] = fe[k][cData[m][l]];
                            if (test(p,pp))
                                boundList[i][j] = boundList[k][m] = 1;
                        }
            }
    }
    numFaces = 0;
    for (i = 0; i < (int)fe.size1(); i++)
        for (j = 0; j < num1; j++)
            if (boundList[i][j] == 0)
                numFaces++;

    surface.resize(numFaces,num2 + 2);
    numFaces = 0;
    for (i = 0; i < (int)fe.size1(); i++)
    {
         for (j = 0; j < num1; j++)
            if (boundList[i][j] == 0)
            {
                cx = x[fe[i][cData[j][4]]];
                cy = y[fe[i][cData[j][4]]];
                cz = z[fe[i][cData[j][4]]];

                x1 = x[fe[i][cData[j][0]]];
                y1 = y[fe[i][cData[j][0]]];
                z1 = z[fe[i][cData[j][0]]];

                x2 = x[fe[i][cData[j][1]]];
                y2 = y[fe[i][cData[j][1]]];
                z2 = z[fe[i][cData[j][1]]];

                x3 = x[fe[i][cData[j][2]]];
                y3 = y[fe[i][cData[j][2]]];
                z3 = z[fe[i][cData[j][2]]];

                for (l = 0; l < num2; l++)
                    data[l] = cData[j][l];

                if ( ((cx-x1)*(y2-y1)*(z3-z1) + (cy-y1)*(z2-z1)*(x3-x1) + (y3-y1)*(cz-z1)*(x2-x1) -
                      (x3-x1)*(y2-y1)*(cz-z1) - (y3-y1)*(z2-z1)*(cx-x1) - (cy-y1)*(z3-z1)*(x2-x1)) > 0)
                {
                    data[0] = cData[j][0];
                    data[1] = cData[j][3];
                    data[2] = cData[j][2];
                    data[3] = cData[j][1];
                }
                for (l = 0; l < num2; l++)
                    surface[numFaces][l] = fe[i][data[l]];
                numFaces++;
            }
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::analyseFunction(void)
{
    FunListDialog* dlg = new FunListDialog(this);
    QString funName;
    bool isFind = false;

    dlg->setup(rList);
    dlg->changeLanguage();
    if (dlg->exec() != QDialog::Accepted) return;

    funName = dlg->getFuncName();
    // Проверка наличия такой функции в уже открытых закладках
    for (int i = 0; i < tabWidget->count(); i++)
        if (tabWidget->tabText(i) == funName)
        {
            isFind = true;
            tabWidget->setCurrentIndex(i);
        }
    if (!isFind)
    {
        tabWidget->addTab(new GLFunWidget(&x,&y,&z,&surface,rList, rList->getFuncNum(funName.toStdString()),this),funName);
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::setRotate(void)
{
    ui->actionRotate->setChecked(true);
    ui->actionScale->setChecked(false);
    ui->actionTranslate->setChecked(false);

    qobject_cast<GLWidget*>(tabWidget->currentWidget())->setRotate();
}
//-------------------------------------------------------------------------------------
void MainWindow::setScale(void)
{
    ui->actionRotate->setChecked(false);
    ui->actionScale->setChecked(true);
    ui->actionTranslate->setChecked(false);
    qobject_cast<GLWidget*>(tabWidget->currentWidget())->setScale();
}
//-------------------------------------------------------------------------------------
void MainWindow::setTranslate(void)
{
    ui->actionRotate->setChecked(false);
    ui->actionScale->setChecked(false);
    ui->actionTranslate->setChecked(true);
    qobject_cast<GLWidget*>(tabWidget->currentWidget())->setTranslate();
}
//-------------------------------------------------------------------------------------
void MainWindow::setupImageParams(void)
{
    SetupImageDialog* iDlg = new SetupImageDialog(this);

    if (qobject_cast<GLWidget*>(tabWidget->currentWidget()))
    {
        iDlg->changeLanguage();
        iDlg->setImageParams(qobject_cast<GLWidget*>(tabWidget->currentWidget())->getImageParams(),true);
        if (iDlg->exec() == QDialog::Accepted)
        {
            qobject_cast<GLWidget*>(tabWidget->currentWidget())->setImageParams(iDlg->getImageParams());
            qobject_cast<GLWidget*>(tabWidget->currentWidget())->repaint();
        }
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::setupRecentActions(void)
{
    for (int i = 0; i < maxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::readSettings(void)
{
//    int numRecentFiles;
    QSettings settings("ZNU","MIRELA");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    QString path = settings.value("path").toString();
    int states = settings.value("state").toInt();

    files = settings.value("recentFileList").toStringList();

    if ((Qt::WindowStates)states == Qt::WindowMaximized)
        this->setWindowState(Qt::WindowMaximized);
    else
    {
        move(pos);
        resize(size);
    }
    setWindowFilePath(path);
}
//-------------------------------------------------------------------------------------
void MainWindow::writeSettings(void)
{
    QSettings settings("ZNU","MIRELA");

    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("path",windowFilePath());
    settings.setValue("state",(int)windowState());
    settings.setValue("recentFileList",files);
}
//-------------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isStarted)
        isStoped = true;
    isDocOpened = false;
    writeSettings();
    event->accept();
}
//-------------------------------------------------------------------------------------
void MainWindow::openRecentFile(void)
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString fileName;

    if (action)
    {
        fileName = action->data().toString();
        loadFile(fileName);
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::updateRecentFileActions(const QString& fileName)
{
    // Удаляем в меню старый список Recent-файлов
    for (int i = 0; i < files.size(); ++i)
        ui->menuFile->removeAction(recentFileActs[i]);

    // Модифицируем список Recent-файлов
    setWindowFilePath(fileName);
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > maxRecentFiles)
        files.removeLast();
    createRecentMenu();
}
//-------------------------------------------------------------------------------------
void MainWindow::createRecentMenu(void)
{
    // Создаем в меню новый список Recent-файлов
    for (int i = 0; i < files.size(); ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());

        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
        recentFileActs[i]->setStatusTip(files[i]);
        ui->menuFile->insertAction(ui->actionExit, recentFileActs[i]);
    }
    if (files.size() > 0)
    {
        separatorAct = ui->menuFile->insertSeparator(ui->actionExit);
        separatorAct->setVisible(files.size() > 0);
    }
}
//-------------------------------------------------------------------------------------
void MainWindow::loadFile(QString& fileName)
{
    QFile file;
    QTextStream in;


    if (isStarted)
    {
        isStoped = true;
        qApp->processEvents();
    }

    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Ошибка открытия файла %1!").arg(fileName));
        return;
    }
    in.setDevice(&file);

    isStoped = false;
    isStarted = true;
    ui->actionStop->setVisible(true);
    pb->show();
    QApplication::setOverrideCursor(Qt::BusyCursor);
    readFile(in);
//    result->setText(in.readAll());
    file.close();
    if (file.error() != QFile::NoError)
    {
        isStarted = false;
        pb->hide();
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, tr("Ошибка"), tr("Ошибка чтения файла %1!").arg(fileName));
        return;
    }
    if (isStoped)
    {
        isStarted = false;
        pb->hide();
        QApplication::restoreOverrideCursor();
        ui->actionStop->setVisible(false);
        return;
    }
    // Формируем КЭ и поверхность
    createFE();
    if (isStoped)
    {
        isStarted = false;
        pb->hide();
        QApplication::restoreOverrideCursor();
        ui->actionStop->setVisible(false);
        return;
    }
    createSurface();
    middleResult();
    isStarted = false;
    QApplication::restoreOverrideCursor();
    pb->hide();
    ui->actionStop->setVisible(false);
    if (isStoped)
        return;

    // Выводим сводку о загруженных данных
    printInfo();
    isStoped = isDocOpened = true;
    setWindowTitle(tr("Постпроцессор системы МІРЕЛА+, ЗНУ, 2014") + " + [" + fileName + "]");
    updateRecentFileActions(fileName);
    checkMenuState();
}
//-------------------------------------------------------------------------------------
void MainWindow::stop(void)
{
    isStoped = true;
}
//-------------------------------------------------------------------------------------
void MainWindow::middleResult(void)
{
    vector<int> middle;
    double sum[6];
    unsigned minIndex;

    // Напряжения S[i][3]-S[i][8] заданы в центрах КЭ. Осредняем их по вершинам эл-тов
    middle.resize(x.size());
    nU.ReSize(x.size(),6);


    pb->setMinimum(0);
    pb->setMaximum(fe.size1() - 1);
    pb->setFormat(tr("Осреднение результатов"));
    for (unsigned i = 0; i < fe.size1(); i++)
    {
        if (isStoped)
            return;
        if (i%10 == 0)
            pb->setValue(i);
        qApp->processEvents();

        sum[0] = sum[1] = sum[2] = sum[3] = sum[4] = sum[5] = 0;
        minIndex = fe[i][0];

        for (unsigned j = 1; j < fe.Size2(); j++)
            if (fe[i][j] < minIndex)
                minIndex = fe[i][j];

        for (unsigned k = 0; k < 6; k++)
            sum[k] =  U[MinIndex][3 + k];

            sum[k] = (*rList)[k + 3].getResults();


        for (DWORD j = 0; j < FE.Size2(); j++)
        {
            for (DWORD k = 0; k < 6; k++)
                nU[FE[i][j]][k] += Sum[k];
            Middle[FE[i][j]]++;
        }
        WinMess();
    }

    for (unsigned i = 0; i < NumVertex; i++)
        for (unsigned j = 3; j < 9; j++)
            if (middle[i])
                U[i][j] = nU[i][j - 3]/double(Middle[i]);
}
