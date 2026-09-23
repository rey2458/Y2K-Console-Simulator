#include "XMB.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    XMB w;
    w.showFullScreen();
    return a.exec();
}