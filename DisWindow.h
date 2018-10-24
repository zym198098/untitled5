#pragma once

#include <QtWidgets/QLabel>
#include <QMutex>
#include<QMenu>
#include<QMouseEvent>
#include<QWheelEvent>
#include"BufSession.h"
#include "HalconCpp.h"
#include <qtimer.h>

using namespace HalconCpp;


class CDispWindow : public QLabel
{
	Q_OBJECT;
public:
	CDispWindow(int id);
	virtual ~CDispWindow(void);
	BOOL  OpenWindow(int ratio_w, int ratio_h);
	void ClearWindow(void);
	Hlong GetWnd()
	{
		return m_hDispWnd;
	}
	void GetWH(int &w, int &h)
	{
		w = this->width();
		h = this->height();
	}
	double  CDispWindow::SetMathExTime();
	void write_message(int Row, int Column, char *sMsg);
	double ResizeImage(int which);
	void  Disp(CBufSession * pBuf);
	void  Disp();
signals:
	// 鼠标单击信号
	void clicked();
	void SigShowStatusMsg(QString msg);
	void SigShowTextMsg(QString msg);
protected:
	void resizeEvent(QResizeEvent *event);
	void mouseReleaseEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * ev);
	//   void mouseDoubleClickEvent(QMouseEvent * ev);
	void wheelEvent(QWheelEvent *event);
	public slots:
	void SlotSavePic(bool);
	void SlotShowContextmenu(const QPoint& p);
	void SlotReProcess(bool);
	void SlotCalcRectanggle(bool);
	void SlotDrawRoi(bool);


public:
	HTuple m_hDispWnd;
	int m_id;
	int m_leftClickFlag;
	CBufSession * m_cacheBuf;
	HObject  ho_Rectangle;
	QTimer *mouseMoveTime;
private:
	int m_last_w;
	int m_last_h;
	int m_x, m_y; //鼠标坐标
	bool m_CalculatFlag = false;
	float m_ratio;
	Hlong m_row1, m_col1, m_row2, m_col2;
	Hlong m_row1_bak, m_col1_bak, m_row2_bak, m_col2_bak;

	HTuple m_iImageWidth, m_iImageHeight;

	HObject m_r, m_g, m_b, m_h, m_s, m_v;
	bool m_isImgCleared = true;
	HTuple  Row1, Column1, Row2, Column2;
	QMenu *m_RightMenu;
	bool m_initFirst;
	int math;
};

