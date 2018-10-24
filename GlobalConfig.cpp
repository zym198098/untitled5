#include "GlobalConfig.h"
#include "Utils.h"
#include <tchar.h>
#include"Log.h"


S_SysCtrl g_SysCtrl;
CGlobalConfig  g_config;
S_Statics  g_statics;
CShmQueue g_queue;

//std::mutex g_get_image_mtx;
//std::condition_variable g_get_image_ok;


void  GQueueInit()
{
    int i=0;
    unsigned  int iDataLen;
    g_queue.Put(100,(char *)&i,sizeof(i));
    g_queue.Get(100,(char *)&i,sizeof(i),iDataLen);

    g_queue.Put(200,(char *)&i,sizeof(i));
    g_queue.Get(200, (char *)&i, sizeof(i), iDataLen);


    g_queue.Put(300,(char *)&i,sizeof(i));
    g_queue.Get(300, (char *)&i, sizeof(i), iDataLen);

    g_queue.Put(400,(char *)&i,sizeof(i));
    g_queue.Get(400, (char *)&i, sizeof(i), iDataLen);
    g_queue.Put(500, (char *)&i, sizeof(i));
    g_queue.Get(500, (char *)&i, sizeof(i), iDataLen);
}


int GQueuePut(int id,void *pBuf, int size)
{
    int iRet=g_queue.Put(id,(char*)pBuf,size);
    if(iRet<0)
    {

        LOGERROR("Error:id:%d,%d\n",id,iRet);
    }
    return iRet;
}


int GQueuePutL(int id,void *pBuf, int size)
{

    int iRet=g_queue.PutL(id,(char *)pBuf,size);
    if(iRet<0)
    {

        LOGERROR("Error:id:%d,%d\n",id,iRet);
    }
    return iRet;
}

int GQueueGet(int id,void *pBuf, unsigned bufsize,unsigned &size)
{

    int iRet=g_queue.Get(id,pBuf,bufsize,size);
    if(iRet<0 && iRet!=-2)
    {
        LOGERROR("Error:id:%d,%d\n",id,iRet);
    }
    return iRet;
}

int GQueueGetL(int id,void *pBuf, unsigned bufsize,unsigned &size)
{

    int iRet=g_queue.GetL(id,pBuf,bufsize,size);
    if(iRet<0 && iRet!=-2)
    {
        LOGERROR("Error:id:%d,%d\n",id,iRet);
    }
    return iRet;
}

CGlobalConfig::CGlobalConfig(void)
{
	bRealTime = 0;

}


CGlobalConfig::~CGlobalConfig(void)
{

} 

int CGlobalConfig::Init()
{
	CheckExpireFlag = 0;
	end_face = 0;
	Roundness_Only = 1;
	xz_dog = 0;
	timeset1 = 0;
	load();
	TupleGenConst(500, -1, &TCfg); //参数配置	
	return 0;
}
void CGlobalConfig::save()
{

}

int CGlobalConfig::load()
{
	CMarkupSTL conf;
	if (!conf.Load("config.xml"))
	{
		LOGERROR("config.xml is not exist");
		return -1;
	}
	if (!conf.FindElem("root"))
	{
		LOGERROR("In config.xml, can not find <root> node");
		return -1;
	}
	conf.IntoElem();
	if (!conf.FindElem("cameras"))
	{
		LOGERROR("In config.xml, can not find <cameras> node");
		return -1;
	}

	while (conf.FindChildElem("entry"))
	{

		int id = atoi(conf.GetChildAttrib("id", "0").c_str());
		string   sSerial = "22112330";   //绑定相机序列号
		mapSerial2Index[sSerial] = id;
		mapIndex2Serial[id] = sSerial;
	}


	if (!conf.FindElem("coms"))
	{
		LOGERROR("In config.xml, can not find <coms> node");
		return -1;
	}

	while (conf.FindChildElem("entry"))
	{
		int id = atoi(conf.GetChildAttrib("id", "0").c_str());
		string   sSerial = conf.GetChildAttrib("序列号");
		mapIndex2comName[id] = sSerial;
	}

	if (!conf.FindElem("spectrums"))
	{
		LOGERROR("In config.xml, can not find <spectrums> node");
		return -1;
	}

	while (conf.FindChildElem("entry"))
	{
		int id = atoi(conf.GetChildAttrib("id", "0").c_str());
		string   sSerial = conf.GetChildAttrib("序列号");
		mapIndex2Spectum[id] = sSerial;
	}


	return 0;
}


