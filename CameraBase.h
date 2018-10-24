#pragma once
#include "HalconCpp.h"
#include "CwxThreadWithQueueBuf.h"
#include "GlobalDef.h"
#include "tshmcommu.h"
#include "BufSession.h"
#include"protocal.pb.h"
#include "Log.h"

#include<map>
using namespace HalconCpp;

using namespace XZProto;


class CCameraBase :	public CwxThreadWithQueueBuf
{
public:
    CCameraBase(int id=0);
    virtual ~CCameraBase(void);
    virtual void* Entry();
public:
    virtual int Init()=0;  //初始化相机
    virtual int Open()=0; //开始采集
    virtual void Close()=0; //关闭相机

    //同步获取一帧，用在同步模式
    virtual int GetOneFrame(void * image);


    int GetCtrlMsg();


    virtual int OnCommand()
    {
        return 0;
    }

    void SetCallBackFlag(bool flag)
    {
        m_bIsUseCallback=flag;
    }

	virtual int SetTriggerMode(bool bTriggleMode) {return 0;};
	virtual int SetIp(int m_id){return 0;};
	virtual void SetexposureTime()=0;
	virtual void SetexposureTime(int) = 0;
private:

    bool m_bIsUseCallback; //相机抓拍是否使用回调函数方式
public:

    int m_id;   //逻辑上的相机id ， 多个相机才有意义

    BOOL m_bDevOpened;       ///< 标识是否已打开设备

//dynamic param
    cam_ctrl_msg  m_ctrl_msg;
    cam_ctrl_msg  m_cfg_msg;
    string  m_sStrCamSerial;
	bool m_bTriggleMode;//触发
};

