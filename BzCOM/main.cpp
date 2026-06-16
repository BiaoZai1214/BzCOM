#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("BzCOM");
    a.setApplicationVersion("1.3");

    MainWindow w;
    w.setWindowIcon(QIcon(":/icons/bz.ico"));
    w.show();
    return QCoreApplication::exec();
}
