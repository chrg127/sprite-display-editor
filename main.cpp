#include <QApplication>
#include <QMainWindow>
#include "window.h"

int main(int argc, char **argv)
{
    QApplication prog(argc, argv);
    Window window;

    prog.setWindowIcon(QIcon("./stuff/icon64.png"));

    window.show();
    return prog.exec();
}
