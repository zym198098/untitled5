#pragma once

#include "HalconCpp.h"
#include"protocal.pb.h"

#include <fstream>
#include <iostream>
#include <map>
#include <QMutex>
#include <QAtomicInt>
#include <stdint.h>
#include <string>
#include <time.h>

using namespace HalconCpp;


using namespace XZProto;
class CBufSession
{
public:
    CBufSession(void);
    virtual ~CBufSession(void);
    void Clear()
    {
        m_cam_serial="none";
        m_iOk=-1;  //status unkonw
        sErrMsg="";
        fps=-1;

	m_Image.GenEmptyObj();
	m_ImageAligned.GenEmptyObj();
	Circle.GenEmptyObj();
	ErrMark.GenEmptyObj();
	ErrMark2.GenEmptyObj();
	Roi.GenEmptyObj();
        m_ctrl_msg.Clear();
        RowCircle= HTuple();
        ColumnCircle= HTuple();

	  //  RowPixCircle= HTuple();
	//ColumnPixCircle= HTuple();
       fRadius=HTuple();

	WOS_Y=HTuple();
	WOS_X= HTuple();
	Class = HTuple();
	Word = HTuple();
	MeasureOut = HTuple();
    }

public:
int  m_id;
cam_ctrl_msg m_ctrl_msg;
std::string    	m_cam_serial;

int 	m_iOk;  //image process result
int 	m_camId;
int 	fps;
int 	per;

HObject  	m_Image;
HObject  	m_ImageAligned;

//img process result
HTuple iRet;
HTuple iRoundness;//Բ��
HTuple fRadius;//ֱ��
HTuple WOS_Y;//����
HTuple WOS_X;//��ת��λ

HTuple RowCircle;
HTuple ColumnCircle;
//HalconCpp::HTuple RowPixCircle;
//HalconCpp::HTuple ColumnPixCircle;
HObject Circle;

HObject ErrMark;
HObject ErrMark2;

HObject Roi;

 //BSF������
 // Local iconic variables
 HObject  ho_Image_BSF, ho_Circle_BSF;

 // Local control variables
 HTuple  hv_CameraParam_BSF, hv_CamInBasePose_BSF;
 HTuple  hv_Row_BSF, hv_Column_BSF, hv_Radius_BSF, hv_XCam_BSF, hv_YCam_BSF, hv_ZCam_BSF;

 HTuple Flag;		//��������� 0Ϊ�޴��󣬸���Ϊ����
 HTuple MeasureOut;//��Ӳ������  ������4��ֵ ��2.22  15.44 11.50  19.33��

 HTuple  Class;  // ocr���
 HTuple  Word;  // ocr���

//string tips
std::string sErrMsg;
QAtomicInt iRef = 0;
};

