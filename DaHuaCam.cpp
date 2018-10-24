#include "DaHuaCam.h"
#include<QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentFilter>

DaHuaCam::DaHuaCam(int id)
:systemObj(CSystem::getInstance())
{
	m_bTriggleMode = false;
	m_bDevOpened = false;
	m_id = id;
	attachGrabbing = 0;
	imagenum = 0;

}


DaHuaCam::~DaHuaCam()
{

}

int DaHuaCam::Init()  //初始化相机
{
	bool isDiscoverySuccess = systemObj.discovery(vCameraPtrList);
	if (!isDiscoverySuccess)
	{
		return -1;//discovery device fail
	}
	if (0 == vCameraPtrList.size())
	{
		return -2;//no camera device find
	}

	cameraSptr = vCameraPtrList[m_id]; // one camera

	for (int i = 0; i < DIM(m_buf); i++)
	{
		char *p = (char *)&m_buf[i];
		GQueuePutL(250 + m_id * 2, (char *)&p, sizeof(char *));
		//LOGERROR("GQueuePut %d",250+m_id*2);
	}

	return 0; // success 
}

int DaHuaCam::Open() //开始采集
{
	LOGERROR("camera connect:%d\n", m_id);
	if (!cameraSptr->connect())
	{
		LOGERROR("camera open failed:%d\n", m_id);
		return -3; // connect cameral fail
	}
	//创建流对象
	SetIp(m_id);

	streamPtr = systemObj.createStreamSource(cameraSptr);
	if (NULL == streamPtr)
	{
		LOGERROR("camera open failed:%d\n", m_id);
		return -4; // create stream obj  fail
	}
	bool isStartGrabbingSuccess = streamPtr->startGrabbing();//开流
	if (!isStartGrabbingSuccess)
	{
		LOGERROR("camera open failed:%d\n", m_id);
		return -5;//StartGrabbing  fail
	}
	SetTriggerMode(true);
	return 0; // success connect
}
void DaHuaCam::Close()//关闭相机
{
	int bRet = 0;
	//停止相机拉流
	if (streamPtr->isGrabbing())
	{
		streamPtr->stopGrabbing();
	}
	//断开设备
	if (attachGrabbing)
	{
		if (m_id == 0)
		{
			bRet = streamPtr->detachGrabbing(onGetFrame0);
			attachGrabbing--;
		}
		else if (m_id == 1)
		{
			bRet = streamPtr->detachGrabbing(onGetFrame1);
			attachGrabbing--;
		}
	}
	if (!cameraSptr->disConnect())
	{
		return; // disConnect cameral fail
	}

}

int DaHuaCam::OnCommand()
{
	IAcquisitionControlPtr sptrAcquisitionControl = systemObj.createAcquisitionControl(cameraSptr);//操作对象
	IAnalogControlPtr sptrAnalogControlPtr = systemObj.createAnalogControl(cameraSptr);//操作对象

	if (NULL == sptrAcquisitionControl)
	{
		return -1;
	}


	if (m_cfg_msg.uint32_cmd() == CMD_RECFG_CMD)
	{

		int iNum = m_cfg_msg.rpt_msg_recfg_param_size();
		for (int i = 0; i < iNum; i++)
		{
			recfg_param * pSetting = m_cfg_msg.mutable_rpt_msg_recfg_param(i);
			int iCamId = pSetting->uint32_camid();

			if (iCamId <= -1) continue;

			if (m_id != i) continue;

			if (pSetting->has_uint32_exp())
			{
				CDoubleNode exposureTime = sptrAcquisitionControl->exposureTime();//曝光数值
				exposureTime.setValue(pSetting->uint32_exp());

			}
			if (pSetting->has_uint32_gain())
			{
				CDoubleNode gainRaw = sptrAnalogControlPtr->gainRaw();//数值
				gainRaw.setValue(pSetting->uint32_gain());
			}
			if (pSetting->has_uint32_rgain())
			{
				//
			}
		}
	}
	return 0;
}

void onGetFrame0(const CFrame &pFrame)
{
	CBufSession *pBuf;
	unsigned per;
	if (g_SysCtrl.run_mode == RUNNING_MODE_UNKNOWN)
	{
		return;
	}
	if (pFrame.valid())
	{

		if (GQueueGetL(250 + 0 * 2, (char *)&pBuf, sizeof(char *), per) < 0)
		{
			return;
		}
		//LOGERROR("GQueueGet 250");
		GenImage1(&pBuf->m_Image, "byte", (Hlong)pFrame.getImageWidth(), (Hlong)pFrame.getImageHeight(), (Hlong)pFrame.getImage());
		pBuf->m_ctrl_msg.set_uint64_sync_time(NowMs());
		pBuf->m_ctrl_msg.set_uint64_frame_time(NowMs());
		GQueuePutL(251 + 0 * 2, (char *)&pBuf, sizeof(char *));
		LOGDEBUG("onGetFrame0");
		//LOGERROR("GQueuePut 251");
	}
	return;
}

void onGetFrame1(const CFrame &pFrame)
{
	CBufSession *pBuf;
	unsigned per;
	if (g_SysCtrl.run_mode == RUNNING_MODE_UNKNOWN)
	{
		return;
	}	
	if (pFrame.valid())
	{
		if (GQueueGetL(250 + 1 * 2, (char *)&pBuf, sizeof(char *), per) < 0)
		{
			return;
		}
		//LOGERROR("GQueueGet 252");
		GenImage1(&pBuf->m_Image, "byte", (Hlong)pFrame.getImageWidth(), (Hlong)pFrame.getImageHeight(), (Hlong)pFrame.getImage());
		pBuf->m_ctrl_msg.set_uint64_sync_time(NowMs());
		pBuf->m_ctrl_msg.set_uint64_frame_time(NowMs());
		GQueuePutL(251 + 1 * 2, (char *)&pBuf, sizeof(char *));
		LOGDEBUG("onGetFrame1");
		//LOGERROR("GQueuePut 253");
	}
	return;
}

int DaHuaCam::GetOneFrame(void * image)//触发一帧
{
	CBufSession *pBuf;
	unsigned per;
	double exposureTimeValue = 0.0;
	while (true)
	{
		if (GQueueGetL(251 + m_id * 2, (char *)&pBuf, sizeof(char *), per) < 0)
		{
			return -7;
		}

		if (m_ctrl_msg.uint64_sync_time()<=pBuf->m_ctrl_msg.uint64_sync_time())
		{
			m_ctrl_msg.set_uint64_sync_time(pBuf->m_ctrl_msg.uint64_sync_time());
			m_ctrl_msg.set_uint64_frame_time(pBuf->m_ctrl_msg.uint64_frame_time());
			int64_t t1, t2;
			t1 = NowMs();

			//LOGDEBUG("set extime mid = %d, ms = %d", m_id, t2 - t1);
			CopyImage(pBuf->m_Image, (HObject *)image);
			t2 = NowMs();
			LOGDEBUG("CopyImage  mid = %d, ms = %d", m_id, t2 - t1);
			break;
		}
		pBuf->Clear();
		GQueuePutL(250 + m_id * 2, (char *)&pBuf, sizeof(char *));
		pBuf = NULL;
		LOGDEBUG("DeleteImage  Timeout  mid = %d", m_id);
	}
	//LOGERROR("GQueuePut %d", 250 + m_id * 2);
	pBuf->Clear();
	GQueuePutL(250 + m_id * 2, (char *)&pBuf, sizeof(char *));
	pBuf = NULL;
	return 0;
}


int  DaHuaCam::SetTriggerMode(bool bTriggleMode)
{
	m_bTriggleMode = bTriggleMode;
	bool bRet;
	IAcquisitionControlPtr sptrAcquisitionControl = systemObj.createAcquisitionControl(cameraSptr);
	if (NULL == sptrAcquisitionControl)
	{
		return -1;
	}
	bRet = streamPtr->stopGrabbing();//停流
	if (!bRet)
	{
		return -2;
	}
	if (bTriggleMode)//硬触发
	{
		CEnumNode enumNode = sptrAcquisitionControl->triggerMode();
		bRet = enumNode.setValueBySymbol("On");
		if (bRet != true)
		{
			return -5;
		}
		// 设置触发源为Line1触发
		enumNode = sptrAcquisitionControl->triggerSource();
		bRet = enumNode.setValueBySymbol("Line1");
		if (bRet != true)
		{
			return -3;
		}
		// 设置触发器
		enumNode = sptrAcquisitionControl->triggerSelector();
		bRet = enumNode.setValueBySymbol("FrameStart");
		if (bRet != true)
		{
			return -4;
		}
		// 设置触发模式

		// 设置外触发为上升沿（下降沿为FallingEdge）
		enumNode = sptrAcquisitionControl->triggerActivation();
		bRet = enumNode.setValueBySymbol("RisingEdge");
		if (bRet != true)
		{
			return -6;
		}
		//注册回调函数	
		if (attachGrabbing == 0)
		{
			if (m_id == 0)
			{
				bRet = streamPtr->attachGrabbing(onGetFrame0);
				if (!bRet)
				{
					return -7;
				}
				attachGrabbing++;
			}
			else if (m_id == 1)
			{
				bRet = streamPtr->attachGrabbing(onGetFrame1);
				if (!bRet)
				{
					return -7;
				}
				attachGrabbing++;
			}
		}
	}
	else//非触发
	{
		CEnumNode enumNode = sptrAcquisitionControl->triggerMode();
		bRet = enumNode.setValueBySymbol("Off");
		if (bRet != true)
		{
			printf("set trigger mode fail.\n");
			return -9;
		}

	}
	bool isStartGrabbingSuccess = streamPtr->startGrabbing();//开流
	if (!isStartGrabbingSuccess)
	{
		return -10;
	}

}

int  DaHuaCam::SetIp(int m_id)
{
	ITransportLayerControlPtr transPtr = systemObj.createTransportLayerControl(cameraSptr);
	CString str(g_config.TCfg[318 + m_id].S());
	transPtr->gevCurrentIPConfigurationPersistentIP().setValue(true);
	if (!transPtr->gevPersistentIPAddress().setValue(str))
	{
		LOGERROR("gevPersistentIPAddress setValue[%s] fail", str.c_str());
		return 0;
	}
	LOGERROR("gevPersistentIPAddress setValue[%s] success", str);
	return 1;
}

void  DaHuaCam::SetexposureTime()
{
	IAcquisitionControlPtr sptrAcquisitionControl = systemObj.createAcquisitionControl(cameraSptr);
	CDoubleNode exposureTime = sptrAcquisitionControl->exposureTime();//曝光数值
	int i = g_config.TCfg[322 + imagenum * 2 + m_id].I();
	exposureTime.setValue(i);
	imagenum = (imagenum + 1) % 2;
	return;
}


void  DaHuaCam::SetexposureTime(int i)
{
	int64_t t1, t2;
	t1 = NowMs();
	IAcquisitionControlPtr sptrAcquisitionControl = systemObj.createAcquisitionControl(cameraSptr);
	CDoubleNode exposureTime = sptrAcquisitionControl->exposureTime();//曝光数值
	exposureTime.setValue(i);
	t2 = NowMs();
	LOGDEBUG("set extime mid = %d, ms = %d,value = %d", m_id, t2 - t1,i);
	return;
}

