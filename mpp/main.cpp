#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    QString translatorFileName = QLatin1String("qt_") + QLocale::system().name();
    QTranslator *translator = new QTranslator(&app);

    // Локализация (стандартных диалогов, e.t.c, ...)
    if (translator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(translator);



    w.show();

    return app.exec();
}
