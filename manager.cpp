
#include "Log.h"
//#include "LocalCam.h"
#include "FileCam.h"
#include "GlobalDef.h"
#include"tshmcommu.h"
#include "Utils.h"
#include"manager.h"
#include"protocal.pb.h"
//#include "Ui.h"
#include "QtUtils.h"
#include<windows.h>
#include <QMessageBox>
using namespace XZProto;

CManager::CManager()
{
    m_iBufSize=40;
    m_isRunning=0;
    m_pCtrlTask=NULL;
    m_pBufSession=NULL;
	memset(m_pCam, 0, sizeof(m_pCam));
    for(int i=0; i<DIM(m_pImgProcessor); i++)
    {
        m_pImgProcessor[i]=NULL;

    }

}


CManager::~CManager(void)
{
    Stop();
}



int CManager::Init()
{


    LOGDEBUG("app start");

    memset((void *)&g_SysCtrl,0,sizeof(S_SysCtrl));
    g_SysCtrl.quit=false;
    //GQueueInit();
    m_pBufSession = new CBufSession[m_iBufSize];
    for(int j=0; j<m_iBufSize; j++)
    {
        m_pBufSession[j].m_id=j;
        char *p=(char *)&m_pBufSession[j];
        GQueuePutL(CAM_PIPE_SHM_ID_BASE, (char *)&p, sizeof(char *));
		//LOGERROR("GQueueGet 100");
    }

	int iProcNum = DIM(m_pImgProcessor);
    for(int i=0; i<iProcNum; i++)
    {
        if( m_pImgProcessor[i] ==NULL)
        {
            m_pImgProcessor[i] =new CImgProcessor(i);
            if( m_pImgProcessor[i]->Init()!=0)
            {
                QMessageBox::critical(NULL,QStringLiteral("错误"),QStringLiteral("算法线程启动失败2"));
                return -3;
            }
            m_pImgProcessor[i]->Execute();
        }
    }


//   m_pDisplay=new CDisplay();
    // m_pDisplay->Execute();

    //when init ,dispaly thread must created

    return 0;
}

int CManager::Stop()
{
   
	
	for (int i = 0; i<DIM(m_pCam); i++)
	{
		if (m_pCam[i] == NULL) continue;
		m_pCam[i]->Delete();//关线程
		Sleep(300);
		//delete m_pCam[i];//释放内存
		m_pCam[i] = NULL;
	}

    return 0;
}



int CManager::Start(int iCamType,int run_mode)
{

    Stop();
    Sleep(300);
	if (run_mode == 2)
	{
		for (int i = 0; i < DIM(m_pCam); i++)
		{
			m_pCam[i] = new CFileCam(i);
		}

	}
	else
	{
		for (int i = 0; i < DIM(m_pCam); i++)
		{
			m_pCam[i] = new DaHuaCam(i);
		}
	}
	for (int i = 0; i<DIM(m_pCam); i++)
	{
		if (m_pCam[i]->Init() != 0)
		{
			QMessageBox::critical(NULL, QStringLiteral("错误"), QStringLiteral("相机启动失败"));
			return -2;
		}

		m_pCam[i]->Execute();
	}
	LOGDEBUG("init succeed\n");
    //we must set camara when init ok
    return 0;
}


void CManager::CtrlThread(void * pOwner)
{

    return ;

}


int  CManager::RefreshFileCamFileList(std::string sFilePath,std::vector<std::string> &  vfiles_log,std::vector<std::string>&  vfiles_pos)
{
    vfiles_log.clear();
    vfiles_pos.clear();

    GetAllFilePath(sFilePath,vfiles_log);

    vector<string>::iterator itr = vfiles_log.begin();
    while (itr!=vfiles_log.end())
    {
        string filename=*itr;
        string s=filename.substr(filename.size()-7,filename.size());

        if (stricmp(s.c_str(),"pos.jpg")==0||stricmp(s.c_str(),"pos.bmp")==0)
        {
            vfiles_pos.push_back(filename);
            itr=vfiles_log.erase(itr);
        }
        else if(stricmp(s.c_str(),"log.jpg")!=0&&stricmp(s.c_str(),"log.bmp")!=0)
        {
            itr=vfiles_log.erase(itr);
        }
        else
        {
            itr++;
        }
    }
    return 0;
}



int  CManager::SetTriggerMode(bool bTriggleMode)
{
	int iRet;
	for (int i = 0; i < DIM(m_pCam); i++)
	{
		if (m_pCam[i] != NULL)
		{
			iRet = m_pCam[i]->SetTriggerMode(bTriggleMode);
		}
	}
	return iRet;
}

void  CManager::SetIp(int m_id)
{
	int iRet;
	if (m_pCam[m_id] != NULL)
	{
		iRet = m_pCam[m_id]->SetIp(m_id);
	}
	return;
}

void  CManager::SetexposureTime()
{
	for (int i = 0; i < DIM(m_pCam); i++)
	{
		if (m_pCam[i] != NULL)
		{
			m_pCam[i]->SetexposureTime();
		}
	}
	return;
}

void  CManager::SetexposureTime(int i, int exposureTime)
{
	if (m_pCam[i] != NULL){
		m_pCam[i]->SetexposureTime(exposureTime);
	}
	return;
}
