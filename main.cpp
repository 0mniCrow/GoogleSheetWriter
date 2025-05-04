#include "googlesheetsmodifier.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GoogleSheetsModifier w;
    w.show();

    return a.exec();
}
