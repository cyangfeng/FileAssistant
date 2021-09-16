#include "fileassistant.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileAssistant w;
    w.setWindowTitle(QStringLiteral("ÎÄ¼şÖúÊÖ V1.0"));
    w.setWindowIcon(QIcon("image/bitbug.ico"));
    w.show();
    return a.exec();
}
