#include "fileassistant.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    using namespace fileassistant;

    QApplication a(argc, argv);
    FileAssistant w;
    w.setWindowTitle(QStringLiteral("文件助手 V1.0.3"));
    w.setWindowIcon(QIcon("image/bitbug.ico"));

	QPalette paletteL;
	paletteL.setBrush(QPalette::Background, QBrush(Qt::white));
	w.setPalette(paletteL);//设置背景
    w.show();

    return a.exec();
}
