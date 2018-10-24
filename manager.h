
#pragma once
#include "CameraBase.h"
#include "ImgProcessor.h"
#include "GlobalConfig.h"
#include "task_thread.h"
#include "dahuacam.h"

#include <vector>
#include <QtWidgets/QWidget>
#include "GlobalDef.h"

class CManager
{
public:
    CManager();
    virtual ~CManager(void);
    int Start(int iCamType, int run_mode);
    int Stop();
    int Init();

    void ResetCtrlChanel(int run_mode,int input_source);
    static void CtrlThread(void *);
	int  SetTriggerMode(bool bTriggleMode);
	void  SetIp(int m_id);
    int  RefreshFileCamFileList(std::string sFilePath,std::vector<std::string> &  vfiles_log,std::vector<std::string>&  vfiles_pos);
	void  SetexposureTime();
	void  SetexposureTime(int,int);
private:
    int m_isRunning;
    CCameraBase * m_pCam[2];	//多个相机
    int m_iBufSize;



public:

    //  CDisplay *m_pDisplay;  //this must be public
    CImgProcessor * m_pImgProcessor[2];


    CBufSession * m_pBufSession;

    TaskThread * m_pCtrlTask;

};
