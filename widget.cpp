#include "widget.h"
#include <QGridLayout>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include <QMessageBox>


using namespace XZProto;
using namespace HalconCpp;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
	g_config.Init();
	createUi();
	init();
}

Widget::~Widget()
{

}
void Widget::createUi()
{
	Btn_read = new QPushButton("read imaage", this);
	Btn_chuli = new  QPushButton("chuli image", this);
	btn_start_cam = new QPushButton(QStringLiteral("启动相机"), this);
	
	camInfo = new QLineEdit( this);
	camInfo->setFixedWidth(this->width()*0.5);
	lab1 = new QLabel();
	QHBoxLayout *hlayout = new QHBoxLayout;
	
	hlayout->addWidget(btn_start_cam);
	hlayout->addWidget(camInfo);
	hlayout->addWidget(Btn_read);
	hlayout->addWidget(Btn_chuli);
	QVBoxLayout *vlayout = new QVBoxLayout(this);
	
	DispGridLayout = new QGridLayout();
	creatDisWindow();
	//Disp = new QHalconWindow(this);
	//Disp->setMinimumSize(600, 400);
	//vlayout->addWidget(Disp);
	vlayout->addLayout(DispGridLayout);
	vlayout->addLayout(hlayout);
	connect(Btn_read, SIGNAL(clicked()), SLOT(btn_read()));
	connect(Btn_chuli, SIGNAL(clicked()), SLOT(btn_chuli()));
	connect(btn_start_cam, SIGNAL(clicked()), SLOT(startCam()));
}

void Widget::InitFg(void)
{
	using namespace HalconCpp;

	// Open framegrabber and grab initial image
	OpenFramegrabber("File", 1, 1, 0, 0, 0, 0, "default", -1, "default", -1, "default",
		"board/board.seq", "default", -1, 1, &FGHandle);
	GrabImage(&Image, FGHandle);
	Image.GetImageSize(&ImageWidth, &ImageHeight);
	Disp->GetHalconBuffer()->SetPart(0, 0, ImageHeight - 1, ImageWidth - 1);
	Disp->GetHalconBuffer()->SetLineWidth(3);
	Disp->GetHalconBuffer()->DispObj(Image);
	Disp->GetHalconBuffer()->FlushBuffer();
}

void Widget::btn_read()
{
	ReadImage(&ho_Image, "E:/公牛图片/Pics/B205/G06K311Y/0525/20180521092124/0_1_20180521092124.775_G06K311Y.jpg");
	GetImageSize(ho_Image, &hv_Width, &hv_Height);
	ImageHeight = hv_Width;
	ImageWidth = hv_Height;
	pbufimage1 = new CBufSession();
	pbufimage1->m_ImageAligned = ho_Image;
	m_dispWindow[0]->Disp(pbufimage1);

	/*Disp->GetHalconBuffer()->SetPart(0, 0, ImageWidth - 1, ImageHeight - 1);

		Disp->GetHalconBuffer()->DispObj(ho_Image);
		Disp->GetHalconBuffer()->FlushBuffer();*/
	
}
int Widget::init()
{
	/*gLog.OpenLogFile("sys.log");
	gLog.SetMaxLogFileSize(100 * 1024 * 1024);
	gLog.SetLogLevel((XZClog::LOG_LEVEL)g_config.TCfg[302].I());*/
	m_pManager = new CManager();
	if (m_pManager->Init() < 0)
	{
		return -2;
	}

	return 0;
}
void Widget::creatDisWindow()
{
	
	QStringList sNameList = QStringList() << QStringLiteral("正面") << QStringLiteral("背面");

	for (int i = 0; i < 2; i++)
	{
		m_pDisDockWidget[i] = new QDockWidget(this);
		m_dispWindow[i] = new CDispWindow(i);
		m_dispWindow[i]->setText("");
		m_dispWindow[i]->setStyleSheet(QStringLiteral("background-color: rgb(206, 206, 206);"));
		m_dispWindow[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_dispWindow[i]->setVisible(true);
		m_pDisDockWidget[i]->setWindowTitle(sNameList[i]);
		m_pDisDockWidget[i]->setWidget(m_dispWindow[i]);
		m_pDisDockWidget[i]->setContentsMargins(0, 0, 0, 0);
		m_pDisDockWidget[i]->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
		/*connect(m_dispWindow[i], &CDispWindow::SigShowStatusMsg, this, [=](QString s)
		{
			m_pLabelAbsNorvmalStatus->setText(s);
		}
		);
		connect(m_dispWindow[i], &CDispWindow::SigShowTextMsg, this, [=](QString s)
		{
			RealTimeDebugMsg(s, "green");
		}
		);*/
		DispGridLayout->addWidget(m_pDisDockWidget[i], i / 2, i % 2);
		DispGridLayout->setColumnStretch(0, 1);
		DispGridLayout->setColumnStretch(1, 1);
	}
}
void Widget::btn_chuli()
{

}

int Widget::startCam()
{
	int iRet;
	/*int i = g_config.TCfg[301].I();

g_SysCtrl.input_source = g_config.TCfg[301].I();
	
	camInfo->setText((QString)i);
*/
	cam_ctrl_msg  ctrl_msg;
	char buf[4096];
	try {



		Sleep(10);
		iRet = m_pManager->Start(g_config.TCfg[301].I(), RUNNING_MODE_ONLINE);
		Sleep(10);
		if (iRet < 0)
		{
			return iRet;
		}



		//启动的时候设置相机参数
		
		
	}
	catch (...)
	{
		QMessageBox::critical(NULL, QStringLiteral("错误"), QStringLiteral("相机启动失败！"));
		return -1;
	}
	return 0;
}
