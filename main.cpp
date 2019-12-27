#include "CG_Task_ZC_QT.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CG_Task_ZC_QT w;
	w.show();
	return a.exec();
}
