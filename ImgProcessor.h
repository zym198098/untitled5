#pragma once
#include "CwxThreadWithQueueBuf.h"
#include "tshmcommu.h"
#include "BufSession.h"
#include "HalconCpp.h"

using namespace HalconCpp;
class CImgProcessor :public CwxThreadWithQueueBuf
{

Q_OBJECT
public:
    CImgProcessor(int id);
    virtual ~CImgProcessor(void);
    virtual void* Entry();
    virtual int Init();
	int GetCtrlMsg();
signals:
	void SignalDealBuf(int);

private:
    int m_id;
	cam_ctrl_msg  m_ctrl_msg;

	HObject ho_ROI_E;
	HTuple hv_VarModelID;
	HTuple hv_ShapeModelID;
	HTuple hv_ShapeModelRow;
	HTuple hv_ShapeModelColumn;

	HObject ho_RegionDiff;
	HObject ho_ErrorMarkerGap;
	HObject ho_ErrorMarkerBurr;
	HObject ho_PErrorMark;
	HTuple hv_GapMax;
	HTuple hv_BurrMax; 
	HTuple hv_m_iOk;
	HTuple hv_sErrMsg;
	HTuple hv_NumberDefectsGap;
	HTuple hv_NumberDefectsBlur;

	HObject ho_m_Image;
	HObject ho_ROI;
	HObject ho_ScrewROI;
	HObject ho_SNROI;
	HTuple hv_ModelID;
	HTuple hv_ModelRow;
	HTuple hv_ModelColumn;

	HTuple hv_Erosion_Radius;
	HTuple hv_Logo_View_Width;
	HTuple hv_GapTolerance;
	HTuple hv_BurrTolerance;
};

