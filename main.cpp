#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Qt6 默认多启用高DPI支持，也可以显式声明：
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}