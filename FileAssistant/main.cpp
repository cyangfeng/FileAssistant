#include "fileassistant.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    using namespace fileassistant;

    QApplication a(argc, argv);
    FileAssistant w;
    w.setWindowTitle(QStringLiteral("�ļ����� V1.0.3"));
    w.setWindowIcon(QIcon("image/bitbug.ico"));

	QPalette paletteL;
	paletteL.setBrush(QPalette::Background, QBrush(Qt::white));
	w.setPalette(paletteL);//���ñ���
    w.show();

    return a.exec();
}
