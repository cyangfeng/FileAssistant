#include "fileassistant.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileAssistant w;
    w.setWindowTitle(QStringLiteral("�ļ�����"));
    w.setWindowIcon(QIcon("image/shell.png"));
    w.show();
    return a.exec();
}
