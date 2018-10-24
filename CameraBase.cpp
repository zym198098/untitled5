#include "CameraBase.h"
#include "Log.h"
#include <iostream>
#include "GlobalConfig.h"
#include <QtGui>
#include <QMessageBox>
#include"oi_debug.h"
#include <condition_variable>
#include <iostream>
#include <mutex> 
using namespace std;
using namespace XZProto;

CCameraBase::CCameraBase(int id )
{
    m_bIsUseCallback=0;
    m_bDevOpened=false;
    m_id=id;
	m_bTriggleMode = true;
}


CCameraBase::~CCameraBase(void)
{
    // wait();
}


int CCameraBase::GetCtrlMsg()
{
	int iRet = 0;
	char buf[4096];
	unsigned size, flow;
	iRet = GQueueGetL(CAM_CTRL_PIPE_SHM_ID_BASE + m_id, (void *)buf, sizeof(buf), size);
	if (iRet >= 0)
	{
		cam_ctrl_msg  msg;
		if (!msg.ParseFromArray(buf, size))
		{
			LOGERROR("camera Parse  ctrl msg  Fail!");
			return -1;
		}
		//   LOGINFO("cambase get ctrl:%d,%d,%d,need_proc:%d",msg.uint32_cmd(),msg.uint32_group(),msg.uint32_seq(),msg.uint32_need_img_process());

		if (msg.uint32_cmd() == CMD_RECFG_CMD)
		{
			m_cfg_msg = msg;
		}
		else
		{
			LOGDEBUG("cam:%d get ctrl msg,cmd=%d", m_id, msg.uint32_cmd());
			m_ctrl_msg.CopyFrom(msg);
			return 1;

		}
		return 0;
	}
	return -2;

}


int CCameraBase::GetOneFrame(void * image)
{
    return -1;
}


void* CCameraBase::Entry()
{
	int iSeq;
	int iRet = 0;
	int iBufId;
	char buf[4096];
	unsigned size, flow;
	CBufSession * pBuf = NULL;
	unsigned int per;
	if ((iRet = Open()) < 0)
	{
		//QMessageBox::critical(NULL,QStringLiteral("错误"), QStringLiteral("打开相机失败"));
		LOGERROR("camera open failed:%d\n", m_id);
		Close();
		return NULL;
	}
	//清空缓存在管道内的控制字

	while (!TestDestroy() && (!g_SysCtrl.quit))
	{
		if ((iRet = GetCtrlMsg()) == 0)
		{
			iRet = OnCommand(); //set cam param etc ..
			if (iRet < 0)
			{
				Sleep(10);
				continue;
			}
		}
		//  m_ctrl_msg.set_uint32_cmd(CMD_CAP);

		if (pBuf == NULL)
		{
			if (GQueueGetL(CAM_PIPE_SHM_ID_BASE, (char *)&pBuf, sizeof(char *), per) < 0)
			{
				//LOGERROR("GQueueGetL 100 error");
				Sleep(5);
				pBuf = NULL;
				continue;
			}
			LOGERROR("GQueueGetL 100");

			if (pBuf->iRef != 0)
			{
				//还有程序在用这个缓冲
				GQueuePutL(CAM_PIPE_SHM_ID_BASE, (char *)&pBuf, sizeof(char *));
				//LOGERROR("GQueuePutL 100");
				pBuf = NULL;
				continue;
			}
			LOGDEBUG("bufid:%d,%s,seq=%d", pBuf->m_id, pBuf->m_cam_serial.c_str(), pBuf->m_ctrl_msg.uint32_seq());
			pBuf->Clear();
		}
		int seq = m_ctrl_msg.uint32_seq();
		int img_process = m_ctrl_msg.uint32_need_img_process();
		if (img_process == 0 && m_bTriggleMode == true)
		{
			Sleep(10);
			continue;
		}
		if (img_process == 1)
		{
			SetexposureTime(g_config.TCfg[322 + seq]);
		}
		LOGERROR("image one ,pBuf->m_id: %d,camid:%d,iGroup:%d,seq:%d,iNeedProcess:%d", pBuf->m_id, m_id, m_ctrl_msg.uint32_group(), m_ctrl_msg.uint32_seq(), m_ctrl_msg.uint32_need_img_process());

		//	LOGDEBUG("%d",pBuf->m_id);
		try
		{
			int iTry = 50;
			while (iTry > 0)
			{
				if ((iRet = GetOneFrame((void *)&pBuf->m_Image)) < 0)
				{

					Sleep(10);
					iTry--;
					continue;
				}
				//LOGERROR("get image::%d", m_id);
				break;
			}
			if (iTry == 0)
			{
				LOGERROR("GetOneFrame failed: %d", m_id);
				m_ctrl_msg.set_uint32_need_img_process(0);

				continue;
			}

			if (m_ctrl_msg.uint32_need_feed_back())
			{
				GQueuePutL(CAM_CTRL_FEED_BACK, (char *)&m_id, sizeof(m_id));
			}

			m_ctrl_msg.clear_debug_file_name2();
			pBuf->m_ctrl_msg.CopyFrom(m_ctrl_msg);

			pBuf->per = per;
		}
		catch (...)
		{

			LOGERROR("GetOneFrame failed2: %d", m_id);
			continue;
		}

		//if (pBuf->m_ctrl_msg.uint32_cmd() != CMD_CAP)
		//{
		//	LOGERROR("cam  buf,cmd:%d,mgroup:%d,seq:%d, camid:%d", pBuf->m_ctrl_msg.uint32_cmd(), pBuf->m_ctrl_msg.uint32_group(), pBuf->m_ctrl_msg.uint32_seq(), m_id);

		//}



		//if (pBuf->m_ctrl_msg.uint32_need_img_process())
		//{
		//	LOGERROR("cam  bufId %d,cmd:%d,mgroup:%d,seq:%d", pBuf->m_id, pBuf->m_ctrl_msg.uint32_cmd(), pBuf->m_ctrl_msg.uint32_group(), pBuf->m_ctrl_msg.uint32_seq());
		//}

		LOGERROR("image one ,pBuf->m_id: %d,camid:%d,iGroup:%d,seq:%d,iNeedProcess:%d", pBuf->m_id, m_id, pBuf->m_ctrl_msg.uint32_group(), pBuf->m_ctrl_msg.uint32_seq(), pBuf->m_ctrl_msg.uint32_need_img_process());

		if (pBuf != NULL)
		{
			GQueuePutL(IMG_PIPE_SHM_ID_BASE + m_id, (char *)&pBuf, sizeof(char *));
		}

		if (m_ctrl_msg.uint32_trigger_mode())
		{
			Sleep(g_config.TCfg[305].I());
			//Sleep(100);
			//LOGDEBUG("set trigger mode cmd_unkown");
			m_ctrl_msg.set_uint32_cmd(CMD_UNKNOWN); //stop get frame
		}
		m_ctrl_msg.set_uint32_need_img_process(0);
		pBuf = NULL;

	}

	if (pBuf != NULL)
	{
	
		GQueuePutL(IMG_PIPE_SHM_ID_BASE + m_id, (char *)&pBuf, sizeof(char *));
	}
	Close();
	return NULL;
}