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
    virtual int Init()=0;  //��ʼ�����
    virtual int Open()=0; //��ʼ�ɼ�
    virtual void Close()=0; //�ر����

    //ͬ����ȡһ֡������ͬ��ģʽ
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

    bool m_bIsUseCallback; //���ץ���Ƿ�ʹ�ûص�������ʽ
public:

    int m_id;   //�߼��ϵ����id �� ��������������

    BOOL m_bDevOpened;       ///< ��ʶ�Ƿ��Ѵ��豸

//dynamic param
    cam_ctrl_msg  m_ctrl_msg;
    cam_ctrl_msg  m_cfg_msg;
    string  m_sStrCamSerial;
	bool m_bTriggleMode;//����
};

