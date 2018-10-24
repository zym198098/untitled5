#pragma once
#include "CameraBase.h"

class CFileCam   : public CCameraBase
{
public:
	CFileCam(int id);
    virtual ~CFileCam(void);
    virtual int Init();
    virtual int Open();
    virtual void Close();
    virtual int OnCommand();
    virtual int GetOneFrame(void * image);
	virtual int SetTriggerMode(bool bTriggleMode){ return 0; };
	virtual int SetIp(int m_id){ return 0; };
	virtual void SetexposureTime(){ return; };
	virtual void SetexposureTime(int){ return; };

private:

};

