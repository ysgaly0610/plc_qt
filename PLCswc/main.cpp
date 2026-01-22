#include "mainwindow.h"
#include "thread.h"

//QApplication应用程序类
#include <QApplication>

extern QString global_string;

int main(int argc, char *argv[])
{
    //有且只有一个应用程序类的对象
    QApplication a(argc, argv);

    //MainWindow继承于QMainWindow,QMainWindow是一个窗口基类，所以MyWidget也是一个窗口类
    MainWindow w;   //w就是一个窗口
    w.setWindowTitle(QString("iMES-by Image")); //设置标题
    w.show();    //窗口默认是隐藏的，要显示出来
    return a.exec();   //等于a.exec()，return 0。作用让程序一直执行，等待用户操作，等待事件发生
}


