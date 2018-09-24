#include "GraphicTree.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QtBtree w;
    w.show();
    return a.exec();
}
