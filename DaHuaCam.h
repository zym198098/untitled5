#pragma once
//#include "ui.h"
#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/GigE/GigECamera.h"
#include "GenICam/GigE/GigEInterface.h"
#include "Infra/PrintLog.h"
#include "Memory/SharedPtr.h"
#include <string>
#include "CameraBase.h"
#include "oi_debug.h"
#include "GlobalConfig.h"
using namespace Dahua::GenICam;
using namespace Dahua::Infra;

#include "Utils.h"
class DaHuaCam : public CCameraBase
{
public:
	DaHuaCam(int id = 0);
	~DaHuaCam();
public:
	virtual int Init();  //��ʼ�����
	virtual int Open(); //��ʼ�ɼ�
	virtual void Close(); //�ر����							 
	virtual int GetOneFrame(void * image); //ͬ����ȡһ֡������ͬ��ģʽ
	virtual  int OnCommand();//���ع�


	int SetTriggerMode(bool bTriggleMode);//����Ӳ����or�Ǵ���
	int SetIp(int m_id);
	void SetexposureTime();
	void SetexposureTime(int);
	CBufSession m_buf[10]; //10������
private:
	CFrame frame;
	ICameraPtr cameraSptr;
	CSystem &systemObj;
	TVector<ICameraPtr> vCameraPtrList;
	//����������
	IStreamSourcePtr streamPtr;
	int attachGrabbing ;
	int imagenum;

};
void onGetFrame0(const CFrame &pFrame);
void onGetFrame1(const CFrame &pFrame);
