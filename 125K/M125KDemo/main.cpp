#include "mainwindow.h"
#include <QApplication>
#include "QLibrary"
#include "QMessageBox"
/**
 * @brief main
 * @param argc
 * @param argv
 * @return exit时的代码
 * Qt应用程序的入口
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLibrary mylib("M125Dll.dll");
    if(mylib.load())
    {
         QMessageBox::information(NULL,"OK","DLL load is OK!");
    }
    else
    {
         QMessageBox::information(NULL,"NO","DLL load is OK!");
    }
    MainWindow w;
    w.show();

    return a.exec();
}
