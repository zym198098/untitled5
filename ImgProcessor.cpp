#include "ImgProcessor.h"
#include "BufSession.h"
#include "GlobalDef.h"
#include "Log.h"
#include"Utils.h"
#include "oi_debug.h"
#include "GlobalConfig.h"
#include "protocal.pb.h"
//#include "Ui.h"
#include <condition_variable>
#include <iostream>
#include <mutex>
using namespace XZProto;
void MyHalconExceptionHandler(const HalconCpp::HException& except)
{
  //  LOGDEBUG("catch exception:%s,%d,%s,%s", except.file, except.line,except.proc,except.message);

    throw except;
}
CImgProcessor::CImgProcessor(int id):CwxThreadWithQueueBuf()
{
    m_id=id;
    //HException::InstallHHandler(&MyHalconExceptionHandler);
}





CImgProcessor::~CImgProcessor(void)
{}

int CImgProcessor::Init()
{

    return 0;
}

int CImgProcessor::GetCtrlMsg()
{
	int iRet = 0;
	char buf[4096];
	unsigned size, flow;
	iRet = GQueueGetL(IMG_CTRL_PIPE_SHM_ID_BASE + m_id, (void *)buf, sizeof(buf), size);
	if (iRet >= 0)
	{
		cam_ctrl_msg  msg;
		if (!msg.ParseFromArray(buf, size))
		{
			LOGERROR("camera Parse  ctrl msg  Fail!");
			return -1;
		}
		LOGDEBUG("img process get one ctrl msg,m_id:%d", m_id);
		m_ctrl_msg.CopyFrom(msg);

		return 0;
	}
	return -1;

}

void* CImgProcessor::Entry()
{

	int iBufId;
	int iRet;
	CBufSession * pBuf = NULL;
	Hlong iImageWidth, iImageHeight;
	S_result stReslt;
	int flag;
	unsigned int per;
	HTuple RowDm, ColDm, RadiusDm = 0;
	HTuple good1, good2, good3, good4, good5, good6;
	static HObject diff, roi, image, imageAligned, circle, errMark;
	static HTuple hv_HomMat2D;
	HTuple Flag;
	//while (!TestDestroy() && (!g_SysCtrl.quit))
	//{
	//	if (GQueueGetL(IMG_PIPE_SHM_ID_BASE + m_id, (char *)&pBuf, sizeof(char *), per) < 0)
	//	{
	//		Sleep(10);
	//		continue;
	//	}

	//	GetCtrlMsg();

	//	//LOGDEBUG("%d",pBuf->m_id);

	//	cam_ctrl_msg  & ctrl_msg = pBuf->m_ctrl_msg;

	//	//m_ctrl_msg.set_uint32_cmd(ctrl_msg.uint32_cmd());
	//	//m_ctrl_msg.set_uint64_frame_time(ctrl_msg.uint64_frame_time());
	//	//m_ctrl_msg.set_uint64_sync_time(ctrl_msg.uint64_sync_time());
	//	//m_ctrl_msg.set_uint32_seq(ctrl_msg.uint32_seq());

	//	//m_ctrl_msg.set_uint32_pysic_camid(ctrl_msg.uint32_pysic_camid());
	//	//replace it
	//	//ctrl_msg = m_ctrl_msg;

	//	int iSeq = ctrl_msg.uint32_seq(); //当前图片序号
	//	int iGroup = ctrl_msg.uint32_group();
	//	//int iPos = ctrl_msg.uint32_pos();
	//	//FED_0(pBuf->m_Image, pBuf->Roi, pBuf->ErrMark, &pBuf->Roi, &pBuf->ErrMark, 1, g_config.TCfg, &pBuf->Flag);

	//	//LOGERROR("image one ,pBuf->m_id: %d,camid:%d,iGroup:%d,seq:%d,iNeedProcess:%d", pBuf->m_id, m_id, iGroup, iSeq, ctrl_msg.uint32_need_img_process());
	//	pBuf->Flag = HTuple();
	//	TupleGenConst(100, 0, &pBuf->Flag);
	//	TupleGenConst(100, 0, &Flag);
	//	if (ctrl_msg.uint32_need_img_process())
	//	{
	//		LOGDEBUG("bufid:%d,%s,seq=%d", pBuf->m_id, pBuf->m_cam_serial.c_str(), pBuf->m_ctrl_msg.uint32_seq());
	//	}
	//	if (ctrl_msg.uint32_cmd() != CMD_CAP)
	//	{
	//		//pBuf->m_ctrl_msg.set_uint32_pysic_camid(m_id);
	//		GQueuePutL(UI_PIPE_SHM_ID_BASE, (char *)&pBuf, sizeof(char *));
	//		emit SignalDealBuf(0);
	//		LOGINFO("img drop one:bufid:%d,%d,%d,%d,need_proc:%d", pBuf->m_id, ctrl_msg.uint32_cmd(), ctrl_msg.uint32_group(), ctrl_msg.uint32_seq(), ctrl_msg.uint32_need_img_process());
	//		continue;
	//	}

	//	do{
	//		//算法处理
	//		pBuf->m_iOk = 0;
	//		if (ctrl_msg.uint32_need_img_process() != 1)
	//			break;

	//		try
	//		{
	//			if (iSeq == 0 && g_config.TCfg[326].I())
	//			{
	//				int64_t t1, t2;
	//				t1 = NowMs();
	//				pBuf->m_ImageAligned.Clear();
	//				//防止超车
	//				t2 = NowMs();
	//				if ((t2 - t1) > 20)
	//					LOGERROR("****Seq=%d:****t2-t1===%d", iSeq, t2 - t1);
	//			}
	//			else if (iSeq == 1 && g_config.TCfg[327].I())
	//			{
	//				//同心度检测
	//				int64_t t1, t2;
	//				t1 = NowMs();
	//				if (g_config.TCfg[36].I())
	//				{
	//					GN_PatternMatch_B(pBuf->m_Image, g_config.ImageTemplate_Back, g_config.ROI_Template_Back, &pBuf->m_ImageAligned,
	//						&pBuf->Roi, &pBuf->ErrMark, &pBuf->ErrMark2, g_config.TCfg, pBuf->Flag, &pBuf->Flag);
	//					roi = pBuf->Roi;
	//					image = pBuf->m_Image;
	//					hv_HomMat2D = pBuf->Flag.TupleSelectRange(50, 55);
	//				}
	//				else
	//				{
	//					pBuf->m_ImageAligned.Clear();
	//				}
	//				t2 = NowMs();
	//				if ((t2 - t1) > 20)
	//					LOGERROR("****Seq=%d:****t2-t1===%d", iSeq, t2 - t1);
	//			}
	//			else if (iSeq == 2 && g_config.TCfg[328].I())
	//			{
	//				//同心度检测
	//				int64_t t1, t2;
	//				t1 = NowMs();
	//				if (g_config.TCfg[36].I()) {
	//					GN_PatternMatch_Front(pBuf->m_Image, g_config.ImageTemplate_Front, g_config.ROI_Template_Front,
	//						&pBuf->Roi, &pBuf->ErrMark, &pBuf->ErrMark2, &pBuf->m_ImageAligned, g_config.TCfg, pBuf->Flag, &pBuf->Flag);
	//				}
	//				else
	//				{
	//					pBuf->m_ImageAligned.Clear();
	//				}
	//				t2 = NowMs();
	//				if ((t2 - t1) > 20)
	//					LOGERROR("****Seq=%d:****t2-t1===%d", iSeq, t2 - t1);
	//			}
	//			else if (iSeq == 3 && g_config.TCfg[329].I())
	//			{
	//				//同心度检测
	//				int64_t t1, t2;
	//				t1 = NowMs();
	//				if (g_config.TCfg[36].I()) {
	//					pBuf->Flag[HTuple::TupleGenSequence(50, 55, 1)] = hv_HomMat2D;
	//					Count_Chars(g_config.ROI_Template_Back, pBuf->m_Image, &pBuf->m_ImageAligned, &pBuf->Circle, &pBuf->ErrMark, g_config.TCfg, pBuf->Flag, &pBuf->Flag);
	//					if (pBuf->Flag[2]!=0)
	//					{
	//						Flag[HTuple::TupleGenSequence(50, 55, 1)] = hv_HomMat2D;
	//						Count_Chars(g_config.ROI_Template_Back, image, &imageAligned, &circle, &errMark, g_config.TCfg, Flag, &Flag);
	//						if (Flag[2] == 0)
	//						{
	//							pBuf->Flag = Flag;
	//							pBuf->m_ImageAligned = imageAligned;
	//							pBuf->Circle = circle;
	//							pBuf->ErrMark = errMark;
	//						}
	//					}
	//				}
	//				else
	//				{
	//					pBuf->m_ImageAligned.Clear();
	//				}
	//				t2 = NowMs();
	//				if ((t2 - t1) > 20)
	//					LOGERROR("****Seq=%d:****t2-t1===%d", iSeq, t2 - t1);
	//			}


	//		}
	//		catch (...)
	//		{
	//			pBuf->m_ImageAligned.Clear();
	//			LOGERROR("catch exception ******************:seq=%d", iSeq);
	//		}

	//	} while (0);

	//	if (pBuf != NULL)
	//	{

	//		LOGERROR("image one ,pBuf->m_id: %d,camid:%d,iGroup:%d,seq:%d,iNeedProcess:%d,error flag 2: %d,flag 12: %d,flag 22: %d", pBuf->m_id, m_id, iGroup, iSeq, ctrl_msg.uint32_need_img_process(), pBuf->Flag[2].I(), pBuf->Flag[12].I(), pBuf->Flag[22].I());
	//		//pBuf->m_ctrl_msg.set_uint32_pysic_camid(m_id);
	//		GQueuePutL(UI_PIPE_SHM_ID_BASE, (char *)&pBuf, sizeof(char *));
	//		emit SignalDealBuf(0);
	//	}
	//	pBuf = NULL;
	//	m_ctrl_msg.set_uint32_need_img_process(0);
	//	Sleep(10);
	//	//Do something
	//}

	return NULL;
}
