#include <QApplication>
#include "main_window.h"
#include "tool.h"
#include "sprite.h"
#include <QVariant>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Tool tool;
    MainWindow window(&tool);
    a.setWindowIcon(QIcon("./stuff/icon64.png"));
    window.show();
    return a.exec();
}

