#include "FileCam.h"
#include "Utils.h"
#include "QtUtils.h"
#include <QFileInfo>
#include<QStringList>


CFileCam::CFileCam(int id) : CCameraBase(id)
{
}
CFileCam::~CFileCam(void)
{
}


int CFileCam::OnCommand()
{

    return 0;
}

int CFileCam::Init()
{
    return 0;
}
int CFileCam::Open()
{
    Herror herr;
    if(m_bDevOpened) return 0;
    SetCallBackFlag(false); //同步方式
    return 0;
}

void CFileCam::Close()
{

}

int CFileCam::GetOneFrame(void * image)
{
    try
    {
        if(!m_ctrl_msg.has_debug_file_name1())
        {
            Sleep(100);
            return -1;
        }
         Sleep(100);
        string file=m_ctrl_msg.debug_file_name1();
        QString qfile = QString::fromStdString(file);
        QFileInfo fi(qfile);

        if (fi.exists())
        {

            ReadImage((HObject *)image, qfile.toLocal8Bit().data());

            // QString baseName=  fi.fileName();
            //   QString sSuffix= fi.suffix();
            //    QString sBundleName=	fi.bundleName();
            //   QString s1=fi.completeSuffix();
            //QString sCompleteBaseName=fi.completeBaseName();
            //QStringList list = sCompleteBaseName.split("_");
			//int aInnerId[] = { 0, 0, 1, 1, 2, 2, 2, 3, 3, 2 };   //zyl 公牛项目中，组号放置首位，序号放至次位的修改
			//m_ctrl_msg.set_uint32_pysic_camid(aInnerId[list[1].toInt()]);
            
			//if (list.size() >= 2)
            //{
            //    m_ctrl_msg.set_uint32_seq(list[1].toInt());
           //     m_ctrl_msg.set_uint32_group(list[0].toInt());
           // }
            m_sStrCamSerial="filecam"+itoa(m_ctrl_msg.uint32_pysic_camid());

            //   LOGDEBUG("filecam:get cap cmd seq:%d,filename:%s",m_ctrl_msg.uint32_seq(),m_ctrl_msg.debug_file_name1().c_str());
        }
        else
        {
            return -1;
        }
    }
    catch ( ... )
    {
        return -2;
    }
    return 0;
}

