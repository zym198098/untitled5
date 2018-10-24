#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QPushButton>
#include <qlabel.h>
#include <QGridLayout>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<qlineedit.h>
#include<qgridlayout.h>
#include<qdockwidget.h>
#include<qlabel.h>
#include "qhalconwindow.h"
#include<QGridLayout>
#include"DisWindow.h"
#include<HalconCpp.h>
#include"manager.h"
#include "HalconCpp.h"

#ifndef __APPLE__
#  include "HalconCpp.h"
#  include "HDevThread.h"
#  if defined(__linux__) && !defined(__arm__) && !defined(NO_EXPORT_APP_MAIN)
#    include <X11/Xlib.h>
#  endif
#else
#  ifndef HC_LARGE_IMAGES
#    include <HALCONCpp/HalconCpp.h>
#    include <HALCONCpp/HDevThread.h>
#  else
#    include <HALCONCppxl/HalconCpp.h>
#    include <HALCONCppxl/HDevThread.h>
#  endif
#  include <stdio.h>
#  include <HALCON/HpThread.h>
#  include <CoreFoundation/CFRunLoop.h>
#endif





class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
	QPushButton *Btn_read;
	QPushButton *Btn_chuli;
	QPushButton *btn_start_cam;
	QLineEdit *camInfo;
	QLabel *lab1;
	void createUi();
	//创建两个图像显示窗口
	CDispWindow *m_dispWindow[2];
	QDockWidget   * m_pDisDockWidget[2];
	QGridLayout *DispGridLayout;

	CManager *m_pManager;
	void InitFg(void);
	HalconCpp::HTuple FGHandle;
	HalconCpp::HImage Image;
	HObject  ho_Image;

	// Local control variables
	HTuple  hv_AcqHandle;
	HTuple  hv_Width, hv_Height;
	Hlong ImageWidth, ImageHeight;

int  init();
protected:
	void creatDisWindow();

private:
	QHalconWindow * Disp;

	CBufSession * pbufimage1, *pbufimage2;
	protected slots:
	void btn_read();
	void btn_chuli();
int startCam();
};

#endif // WIDGET_H
