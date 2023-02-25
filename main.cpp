#include "chatgpt.h"
#include <QtWidgets/QApplication>
#include "singleapplication.h"
#include <QMessageBox>

int main(int argc, char* argv[])
{
    SingleApplication a(argc, argv);
    if (!a.getInstanceRunning()) {
        ChatGpt w;
        a.setMainWindow(&w);
        a.setQuitOnLastWindowClosed(false);
        return a.exec();
    }
    else
    {
        return 0;
    }
}
