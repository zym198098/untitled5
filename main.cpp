#include "widget.h"
#include <QApplication>
#include"qdesktopwidget.h"

int main(int argc, char *argv[])
{
	int i = 0;//1为小屏 0为全屏
    QApplication a(argc, argv);
	Widget w;

	

	int DeskWidth = qApp->desktop()->availableGeometry().width();
	
	int DeskHeigth = qApp->desktop()->availableGeometry().height()* 0.95;
	
	if (i == 0)
	{
	
		w.setFixedSize(DeskWidth, DeskHeigth);
		
		w.show();
	}
	else
	{
		w.setFixedSize(1366, 768);
		w.show();
	}
		
  //  w.show();

    return a.exec();
}
