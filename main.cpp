#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("跨平台Socket测试工具");
    w.show();
    return a.exec();
}
