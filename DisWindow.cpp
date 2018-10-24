#include "DisWindow.h"
#include "GlobalDef.h"
#include "GlobalConfig.h"
#include"BufSession.h"
#include "Log.h"
#include"Utils.h"
//#include "protocal.pb.h"
//
//using namespace XZProto;
void CPPExpDefaultExceptionHandler(const HException& except)
{
	throw except;
}
void set_display_font(HTuple WindowHandle, HTuple Size, HTuple Font,
	HTuple Bold, HTuple Slant)
{
	using namespace HalconCpp;

	// Local control variables
	HTuple  OS, Exception, AllowedFontSizes, Distances;
	HTuple  Indices;


	// Install default exception handler

	//HException::InstallHHandler(&CPPExpDefaultExceptionHandler);


	//This procedure sets the text font of the current window with
	//the specified attributes.
	//It is assumed that following fonts are installed on the system:
	//Windows: Courier New, Arial Times New Roman
	//Linux: courier, helvetica, times
	//Because fonts are displayed smaller on Linux than on Windows,
	//a scaling factor of 1.25 is used the get comparable results.
	//For Linux, only a limited number of font sizes is supported,
	//to get comparable results, it is recommended to use one of the
	//following sizes: 9, 11, 14, 16, 20, 27
	//(which will be mapped internally on Linux systems to 11, 14, 17, 20, 25, 34)
	//
	//input parameters:
	//WindowHandle: The graphics window for which the font will be set
	//Size: The font size. If Size=-1, the default of 16 is used.
	//Bold: If set to 'true', a bold font is used
	//Slant: If set to 'true', a slanted font is used
	//
	GetSystem("operating_system", &OS);
	if (0 != (HTuple(Size == HTuple()).TupleOr(Size == -1)))
	{
		Size = 16;
	}
	if (0 != ((OS.TupleSubstr(0, 2)) == HTuple("Win")))
	{
		//set font on Windows systems
		if (0 != (HTuple(HTuple(Font == HTuple("mono")).TupleOr(Font == HTuple("Courier"))).TupleOr(Font == HTuple("courier"))))
		{
			Font = "Courier New";
		}
		else if (0 != (Font == HTuple("sans")))
		{
			Font = "Arial";
		}
		else if (0 != (Font == HTuple("serif")))
		{
			Font = "Times New Roman";
		}
		if (0 != (Bold == HTuple("true")))
		{
			Bold = 1;
		}
		else if (0 != (Bold == HTuple("false")))
		{
			Bold = 0;
		}
		else
		{
			Exception = "Wrong value of control parameter Bold";
			throw HException(Exception);
		}
		if (0 != (Slant == HTuple("true")))
		{
			Slant = 1;
		}
		else if (0 != (Slant == HTuple("false")))
		{
			Slant = 0;
		}
		else
		{
			Exception = "Wrong value of control parameter Slant";
			throw HException(Exception);
		}
		try
		{
			SetFont(WindowHandle, ((((((("-" + Font) + "-") + Size) + "-*-") + Slant) + "-*-*-") + Bold) + "-");
		}
		// catch (Exception)
		catch (HException &HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&Exception);
			throw HException(Exception);
		}
	}
	else
	{
		//set font for UNIX systems
		Size = Size*1.25;
		AllowedFontSizes.Clear();
		AllowedFontSizes[0] = 11;
		AllowedFontSizes[1] = 14;
		AllowedFontSizes[2] = 17;
		AllowedFontSizes[3] = 20;
		AllowedFontSizes[4] = 25;
		AllowedFontSizes[5] = 34;
		if (0 != ((AllowedFontSizes.TupleFind(Size)) == -1))
		{
			Distances = (AllowedFontSizes - Size).TupleAbs();
			TupleSortIndex(Distances, &Indices);
			Size = AllowedFontSizes[HTuple(Indices[0])];
		}
		if (0 != (HTuple(Font == HTuple("mono")).TupleOr(Font == HTuple("Courier"))))
		{
			Font = "courier";
		}
		else if (0 != (Font == HTuple("sans")))
		{
			Font = "helvetica";
		}
		else if (0 != (Font == HTuple("serif")))
		{
			Font = "times";
		}
		if (0 != (Bold == HTuple("true")))
		{
			Bold = "bold";
		}
		else if (0 != (Bold == HTuple("false")))
		{
			Bold = "medium";
		}
		else
		{
			Exception = "Wrong value of control parameter Bold";
			throw HException(Exception);
		}
		if (0 != (Slant == HTuple("true")))
		{
			if (0 != (Font == HTuple("times")))
			{
				Slant = "i";
			}
			else
			{
				Slant = "o";
			}
		}
		else if (0 != (Slant == HTuple("false")))
		{
			Slant = "r";
		}
		else
		{
			Exception = "Wrong value of control parameter Slant";
			throw HException(Exception);
		}
		try
		{
			SetFont(WindowHandle, ((((((("-adobe-" + Font) + "-") + Bold) + "-") + Slant) + "-normal-*-") + Size) + "-*-*-*-*-*-*-*");
		}
		// catch (Exception)
		catch (HException &HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&Exception);
			throw HException(Exception);
		}
	}
	return;
}

CDispWindow::CDispWindow(int id)
{
	//OpenWindow();
	m_hDispWnd = 0;
	m_id = id;
	m_last_w = 0;
	m_last_h = 0;
	m_ratio = 1;
	math = 0;

	m_leftClickFlag = false;
	m_cacheBuf = NULL;
	m_r.GenEmptyObj();
	m_b.GenEmptyObj();
	m_g.GenEmptyObj();
	m_h.GenEmptyObj();
	m_s.GenEmptyObj();
	m_v.GenEmptyObj();
	setMouseTracking(true);
	m_initFirst = false;



	m_RightMenu = new QMenu();

	QAction *aRoiAction = m_RightMenu->addAction(QStringLiteral("ROI绘制"));
	connect(aRoiAction, SIGNAL(triggered(bool)), this, SLOT(SlotDrawRoi(bool)));

	QAction *ascendSortAction = m_RightMenu->addAction(QStringLiteral("保存图片"));
	connect(ascendSortAction, SIGNAL(triggered(bool)), this, SLOT(SlotSavePic(bool)));

	QAction *aReProcAction = m_RightMenu->addAction(QStringLiteral("重新运算"));
	connect(aReProcAction, SIGNAL(triggered(bool)), this, SLOT(SlotReProcess(bool)));

	//QAction *aCalAction = m_RightMenu->addAction(QStringLiteral("计算区域"));
	//connect(aCalAction, SIGNAL(triggered(bool)), this, SLOT(SlotCalcRectanggle(bool)));


	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(SlotShowContextmenu(const QPoint&)));


}

void CDispWindow::SlotReProcess(bool)
{
	if (m_cacheBuf == NULL)
		return;
	cam_ctrl_msg  & ctrl_msg = m_cacheBuf->m_ctrl_msg;
	ctrl_msg.set_uint32_reprocess(1);

	m_cacheBuf->ErrMark.GenEmptyObj();
	m_cacheBuf->Class = HTuple();
	m_cacheBuf->Word = HTuple();
	m_cacheBuf->Flag = HTuple();
	TupleGenConst(100, 0, &m_cacheBuf->Flag);
	m_cacheBuf->Roi.GenEmptyObj();
	//  m_cacheBuf->iRef--;
	GQueuePutL(IMG_PIPE_SHM_ID_BASE + m_cacheBuf->m_ctrl_msg.uint32_inner_camid(), (char *)&m_cacheBuf, sizeof(char *));
	//  m_cacheBuf=NULL;
}


void CDispWindow::SlotSavePic(bool)
{

	if (m_cacheBuf == NULL)
		return;

	cam_ctrl_msg  & ctrl_msg = m_cacheBuf->m_ctrl_msg;

	char filename[128];
	char datatime[32];
	char sPath[256];
	time_t t_sync_time = ctrl_msg.uint64_sync_time() / 1000;
	DateTimeStr_r(&t_sync_time, datatime, 1);

	static  char  subdir[128];
	static int init = 0;
	if (!init)
	{
		init = 1;
		time_t t = time(NULL);
		char buf[64];
		//sprintf(subdir,"%s\\Capture\\",g_config.TCfg[303].S());
		sprintf(subdir, "D:\\pics\\Capture\\");
		Rmkdir(subdir);
	}
	QString SSeq = "0";
	string sExt = "tiff";
	if (g_config.TCfg[300].I() == 0)
	{
		sExt = "jpeg";
		sprintf(filename, "%s.%s_%d_%d_%d_%d.jpg", datatime, SSeq.toStdString().c_str(), ctrl_msg.uint32_pysic_camid(), ctrl_msg.uint32_group(), ctrl_msg.uint32_seq(), ctrl_msg.uint32_product_id());
	}
	else if (g_config.TCfg[300].I() == 1)
	{
		sprintf(filename, "%s.%s_%d_%d_%d_%d.tiff", datatime, SSeq.toStdString().c_str(), ctrl_msg.uint32_pysic_camid(), ctrl_msg.uint32_group(), ctrl_msg.uint32_seq(), ctrl_msg.uint32_product_id());
	}
	else
	{
		sprintf(filename, "%s.%s_%d_%d_%d_%d.png", datatime, SSeq.toStdString().c_str(), ctrl_msg.uint32_pysic_camid(), ctrl_msg.uint32_group(), ctrl_msg.uint32_seq(), ctrl_msg.uint32_product_id());

	}

	sprintf(sPath, "%s%s", subdir, filename);


	WriteImage(m_cacheBuf->m_Image, sExt.c_str(), 0, sPath);

	return;

}

void CDispWindow::SlotCalcRectanggle(bool)
{

	if (m_cacheBuf == NULL)
		return;
	if (m_hDispWnd == 0)
		return;
	m_CalculatFlag = true;

	try
	{
		HObject   R, G, B, Rectangle;

		HTuple  Row, Column, Row1, Column1, Row2, Column2, MeanR, MeanG, MeanB, Deviation, Area;
		//      decompose3(m_cacheBuf->m_Image, &R, &G, &B);
		DrawRectangle1(m_hDispWnd, &Row1, &Column1, &Row2, &Column2);
		GenRectangle1(&Rectangle, Row1, Column1, Row2, Column2);
		Intensity(Rectangle, m_r, &MeanR, &Deviation);
		Intensity(Rectangle, m_g, &MeanG, &Deviation);
		Intensity(Rectangle, m_b, &MeanB, &Deviation);



		DispObj(Rectangle, m_hDispWnd);
		QString sPos = "(" + QString::number(Row1[0].I()) + "," + QString::number(Column1[0].I()) + "," +
			QString::number(Row2[0].I()) + "," + QString::number(Column2[0].I()) + ")";

		QString       sMean = QString("(%1,%2,%3)").arg(MeanR[0].I()).arg(MeanG[0].I()).arg(MeanB[0].I());

		emit SigShowTextMsg(sPos + sMean);


		Intensity(Rectangle, m_h, &MeanR, &Deviation);
		Intensity(Rectangle, m_s, &MeanG, &Deviation);
		Intensity(Rectangle, m_v, &MeanB, &Deviation);
		sMean = QString("hsv:(%1,%2,%3)").arg(MeanR[0].I()).arg(MeanG[0].I()).arg(MeanB[0].I());
		emit SigShowTextMsg(sMean);

	}
	catch (...)
	{
	}
	m_CalculatFlag = false;


}

void CDispWindow::SlotDrawRoi(bool)
{
	m_CalculatFlag = true;
	ResizeImage(7);
	m_CalculatFlag = false;
}
void CDispWindow::SlotShowContextmenu(const QPoint& p)
{

	m_RightMenu->exec(QCursor::pos());

}
CDispWindow::~CDispWindow(void)
{
	if (m_hDispWnd != 0)
	{
		HalconCpp::ClearWindow(m_hDispWnd);
		HalconCpp::ClearWindow(m_hDispWnd);
		m_hDispWnd = 0;
	}
}

void CDispWindow::write_message(int Row, int Column, char *sMsg)
{

	if (m_hDispWnd == 0)
		return;
	SetTposition(m_hDispWnd, Row, Column);
	WriteString(m_hDispWnd, sMsg);

	return;
}
void CDispWindow::ClearWindow(void)
{
	if (m_hDispWnd != 0)
	{
		HalconCpp::ClearWindow(m_hDispWnd);
	}
}
BOOL  CDispWindow::OpenWindow(int ratio_w, int ratio_h)
{
	if (m_hDispWnd != 0)
	{
		//  clear_window(m_hDispWnd);
		CloseWindow(m_hDispWnd);
		m_hDispWnd = 0;
	}
	m_last_w = this->width();
	m_last_h = this->height();

	int w = m_last_w;
	int h = m_last_h;
	SetCheck("~father");

	int row = 0, col = 0;
	if (w*ratio_h > ratio_w*h)
	{
		//too width
		row = 0;
		col = (w - ((float)h)*((float)ratio_w / (float)ratio_h)) / 2;
		w = w - 2 * col;
	}
	else if (w*ratio_h < ratio_w*h)
	{
		//too height
		col = 0;
		row = (h - ((float)w)*((float)ratio_h / (float)ratio_w)) / 2;
		h = h - 2 * row;
	}
	HTuple winId = (Hlong)this->winId();
	HalconCpp::OpenWindow(row, col, w, h, winId, "visible", "", &m_hDispWnd);
	SetCheck("father");
	SetDraw(m_hDispWnd, "margin");
	SetColored(m_hDispWnd, 3);
	//set_display_font(m_hDispWnd, 18, "mono", "true", "false");

	return TRUE;
}
void CDispWindow::resizeEvent(QResizeEvent *event)
{
	if (!m_initFirst)
		return;
	OpenWindow(m_iImageWidth, m_iImageHeight);
	//Disp(m_cacheBuf);

}

void CDispWindow::mouseReleaseEvent(QMouseEvent *ev)//抬起
{
	if (m_CalculatFlag)
	{
		return;
	}
	//  LOGDEBUG("(%d,%d)",ev->pos().rx(),ev->pos().ry());
	if (ev->button() == Qt::LeftButton)
	{
		//左键单击拖动

		m_leftClickFlag = false;
		ResizeImage(5);  //通知停止拖动
	}
	else if (ev->button() == Qt::RightButton)
	{
		//右键缩小
		//  ResizeImage(1);
	}
	else if (ev->button() == Qt::MidButton)
	{
		//还原
		ResizeImage(2);
	}

}


void CDispWindow::mousePressEvent(QMouseEvent * ev)//按下
{
	//   LOGDEBUG("(%d,%d)",ev->pos().rx(),ev->pos().ry());
	if (ev->button() == Qt::LeftButton)
	{
		m_leftClickFlag = true;
	}
	m_x = ev->pos().rx();
	m_y = ev->pos().ry();
	ResizeImage(6);  //通知初始位置
}


void CDispWindow::mouseMoveEvent(QMouseEvent * ev)//移动
{
	//  LOGDEBUG("(%d,%d)",ev->pos().rx(),ev->pos().ry());


	// if(m_leftClickFlag)
	if (ev->buttons() == Qt::LeftButton)
	{
		if (m_CalculatFlag)
			return;

		m_x = ev->pos().rx();
		m_y = ev->pos().ry();

		ResizeImage(3);  //拖动效果

	}
	ResizeImage(4);
}



void CDispWindow::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta() / 8;//滚动的角度，*8就是鼠标滚动的距离
	int numSteps = numDegrees / 15;//滚动的步数，*15就是鼠标滚动的角度
	if (numDegrees > 0)
	{
		ResizeImage(0);
	}
	else
	{
		ResizeImage(1);
	}
	event->accept();      //接收该事件
}






double CDispWindow::ResizeImage(int which)
{
	Hlong   Width, Height;
	HTuple row, col, button;
	HTuple hWnd = m_hDispWnd;
	static  Hlong sLastRow = 0, sLastCol = 0;

	try{


		if (hWnd == 0) return 0;

		//获取鼠标位置
		GetMposition(hWnd, &row, &col, &button);

		if (row < 0 || col < 0)
			return 0;

		int bufIdx = 0;

		Width = m_iImageWidth;
		Height = m_iImageHeight;
		if (Width == 0)
		{
			Width = 1280;
			Height = 960;
		}


		if (which < 4)
		{
			/*HalconCpp::ClearWindow(hWnd);*/

			if (which == 0 || which == 1)
			{

				sLastRow = row;
				sLastCol = col;
				if (which == 0)
				{
					//放大
					m_ratio = m_ratio*1.5;
				}
				if (which == 1)
				{
					//缩小
					m_ratio = m_ratio*0.7;
				}

				if (m_ratio<0.1)
					m_ratio = 0.1;
				if (m_ratio>30)
					m_ratio = 30;

				Width = Width / m_ratio;
				Height = Height / m_ratio;
				Hlong startRow, startCol;
				HTuple row1, col1, row2, col2, oldH, oldW;
				//SetPart(m_hDispWnd, m_row1, m_col1, m_row2, m_col2);
				GetPart(m_hDispWnd, &row1, &col1, &row2, &col2);
				//row1 = m_row1;
				//col1 = m_col1;
				//row2 = m_row2;
				//col2 = m_col2;
				oldH = row2 - row1;
				oldW = col2 - col1;
				if (oldH == 0 || oldW == 0)
				{
					oldH = 1;
					oldW = 1;
				}
				startRow = (row - (row - row1)*Height / oldH).D();

				startCol = (col - (col - col1)*Width / oldW).D();


				m_row1 = startRow;
				m_col1 = startCol;
				m_row2 = startRow + Height;
				m_col2 = startCol + Width;
				ClearWindow();
				Disp();
				//   set_part(hWnd, m_row1, m_col1, m_row2, m_col2);
			}
			else if (which == 2)
			{
				m_ratio = 1;
				m_row1 = 0;
				m_col1 = 0;
				m_row2 = Height;
				m_col2 = Width;
				//Disp(m_cacheBuf);
				//set_part(hWnd, m_row1, m_col1, m_row2, m_col2);
				Disp();
			}
			else if (which == 3)
			{
				//拖放

				HTuple lastRow1, lastCol1, lastRow2, LastCol2;
				GetPart(hWnd, &lastRow1, &lastCol1, &lastRow2, &LastCol2);
				/*lastRow1 = m_row1;
				lastCol1 = m_col1;
				lastRow2 = m_row2;
				LastCol2 = m_col2;
				*/
				Hlong rowDiff = row - sLastRow;
				Hlong colDiff = col - sLastCol;

				m_row1 = (lastRow1 - rowDiff).D();
				m_col1 = (lastCol1 - colDiff).D();
				m_row2 = (lastRow2 - rowDiff).D();
				m_col2 = (LastCol2 - colDiff).D();
				//   set_part(hWnd, m_row1, m_col1, m_row2, m_col2);
				//sLastRow = row;
				//sLastCol = col;

				math++;
				Disp();
			}

			//HalconCpp::ClearWindow(hWnd);



		}
		else if (which == 4)
		{
			//跟踪鼠标移动
			HTuple nChannels = 0;
			if (m_cacheBuf == NULL) return 0;
			CountChannels(m_cacheBuf->m_ImageAligned, &nChannels);
			QString s;
			if (nChannels == 3)
			{
				HTuple GrayvalR, GrayvalG, GrayvalB;
				HObject r, g, b;
				Decompose3(m_cacheBuf->m_ImageAligned, &r, &g, &b);
				GetGrayval(r, row, col, &GrayvalR);
				GetGrayval(g, row, col, &GrayvalG);
				GetGrayval(b, row, col, &GrayvalB);
				if (GrayvalR.TupleLength() == 1 && GrayvalG.TupleLength() == 1 && GrayvalB.TupleLength() == 1)
				{
					s = QString("(%1,%2)RGB(%3,%4,%5)").arg(row[0].I()).arg(col[0].I()).arg(GrayvalR[0].I()).arg(GrayvalG[0].I()).arg(GrayvalB[0].I());
				}
			}
			else
			{
				HTuple GrayvalR;
				GetGrayval(m_cacheBuf->m_ImageAligned, row, col, &GrayvalR);
				if (GrayvalR.TupleLength() == 1)
				{
					s = QString("(%1,%2)(%3)").arg(row[0].I()).arg(col[0].I()).arg(GrayvalR[0].I());
				}
			}
			emit SigShowStatusMsg(s);
			//     Disp(m_cacheBuf,s.toStdString());
		}
		else if (which == 5)
		{
			ClearWindow();
			Disp();
			// bMoveFlag[k]=false;
		}
		else if (which == 6)
		{
			sLastRow = row;
			sLastCol = col;
		}
		else if (which == 7)
		{
			if (m_cacheBuf == NULL) return 0;
			HTuple  Information, Information1;
			GetSystem("width", &Information);
			GetSystem("height", &Information1);
			ResetObjDb(Information, Information1, 0);

			DrawRectangle1(hWnd, &Row1, &Column1, &Row2, &Column2);
			Row1 = Row1 / m_ratio + m_row1;
			Column1 = Column1 / m_ratio + m_col1;
			Row2 = Row2 / m_ratio + m_row1;
			Column2 = Column2 / m_ratio + m_col1;
			GenRectangle1(&ho_Rectangle, Row1, Column1, Row2, Column2);
			DispObj(ho_Rectangle, m_hDispWnd);
			LOGERROR("\n DrawRectangle1:Row1:%d ;Column1:%d ;Row2:%d ;Column2:%d \n", Row1.D(), Column1.D(), Row2.D(), Column2.D());
			Disp(m_cacheBuf);
			return 0;
		}
	}
	catch (...)
	{

	}



	return 0;

}

double  CDispWindow::SetMathExTime()
{
	if (m_cacheBuf == NULL) return 0;
	HTuple  Information, Information1;
	GetSystem("width", &Information);
	GetSystem("height", &Information1);
	ResetObjDb(Information, Information1, 0);

	HTuple  hv_Mean, hv_Deviation;
	//			GenRectangle1(&ho_Rectangle, Row1, Column1, Row2, Column2);
	Intensity(ho_Rectangle, m_cacheBuf->m_ImageAligned, &hv_Mean, &hv_Deviation);
	double dou = hv_Mean.D();

	return hv_Mean.D();
}

void  CDispWindow::Disp()
{
	Disp(m_cacheBuf);
}

void  CDispWindow::Disp(CBufSession * pBuf)
{
	if (pBuf == NULL)
		return;

	if (m_cacheBuf != pBuf)
	{

		// 直接替换缓冲
		if (m_cacheBuf)
			m_cacheBuf->iRef--;
		pBuf->iRef++;
		m_cacheBuf = pBuf;
		m_r.GenEmptyObj();
		m_b.GenEmptyObj();
		m_g.GenEmptyObj();
		m_h.GenEmptyObj();
		m_s.GenEmptyObj();
		m_v.GenEmptyObj();
		m_isImgCleared = true;
	}



	try
	{
		int per;
		cam_ctrl_msg  & ctrl_msg = pBuf->m_ctrl_msg;


		if (!m_initFirst)
		{
			GetImageSize(pBuf->m_ImageAligned, &m_iImageWidth, &m_iImageHeight);
			m_row1 = 0;
			m_col1 = 0;
			m_row2 = m_iImageHeight;
			m_col2 = m_iImageWidth;

			m_initFirst = true;
			OpenWindow(m_iImageWidth, m_iImageHeight);
		}
		if (m_hDispWnd == 0)
			return;

		SetPart(m_hDispWnd, m_row1, m_col1, m_row2, m_col2);



		if (pBuf->m_ImageAligned.IsInitialized())
			DispObj(pBuf->m_ImageAligned, m_hDispWnd);
		else
			DispObj(pBuf->m_Image, m_hDispWnd);

		SetLineWidth(m_hDispWnd, 2);
		if (g_config.TCfg[342].D())
		{
			SetColor(m_hDispWnd, "green");
			if (pBuf->Circle.IsInitialized())
			{
				DispObj(pBuf->Circle, m_hDispWnd);
			}
			if (pBuf->Roi.IsInitialized())
			{
				DispObj(pBuf->Roi, m_hDispWnd);
			}

			SetColor(m_hDispWnd, "blue");
			if (pBuf->ErrMark2.IsInitialized())
			{
				DispObj(pBuf->ErrMark2, m_hDispWnd);
			}
			if (pBuf->sErrMsg != "")
			{
				write_message(10, 10, (char *)pBuf->sErrMsg.c_str());
			}
			SetColor(m_hDispWnd, "blue");
			if (ho_Rectangle.IsInitialized())
			{
				DispObj(ho_Rectangle, m_hDispWnd);
			}
		}
		SetColor(m_hDispWnd, "red");
		if (pBuf->ErrMark.IsInitialized())
		{
			DispObj(pBuf->ErrMark, m_hDispWnd);
		}

	}
	catch (...)
	{
		LOGDEBUG("Catch exception whend disp:%d", m_id);

	}
	return;
}

