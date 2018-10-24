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
	virtual int Init();  //初始化相机
	virtual int Open(); //开始采集
	virtual void Close(); //关闭相机							 
	virtual int GetOneFrame(void * image); //同步获取一帧，用在同步模式
	virtual  int OnCommand();//调曝光


	int SetTriggerMode(bool bTriggleMode);//设置硬触发or非触发
	int SetIp(int m_id);
	void SetexposureTime();
	void SetexposureTime(int);
	CBufSession m_buf[10]; //10个缓存
private:
	CFrame frame;
	ICameraPtr cameraSptr;
	CSystem &systemObj;
	TVector<ICameraPtr> vCameraPtrList;
	//创建流对象
	IStreamSourcePtr streamPtr;
	int attachGrabbing ;
	int imagenum;

};
void onGetFrame0(const CFrame &pFrame);
void onGetFrame1(const CFrame &pFrame);
