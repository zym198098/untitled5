#pragma once
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <string>
#include"tshmcommu.h"
#include "GlobalDef.h"
#include"sqlite3.h"
#include "HalconCpp.h"
#include "CppSQLite3.h"
//#include "Algorithm.h"
#include <condition_variable>
#include <iostream>
#include <mutex>
#include"MarkupSTL.h"
using namespace std;
using namespace HalconCpp;
enum
{
    SAVE_MODE_NONE=0,
    SAVE_MODE_OK=1,
    SAVE_MODE_NG=2,
    SAVE_IN_SINGLE_PATH=0x80
};
#define  PIC_WIDTH 2592
#define POS_VIEW_WIDETH   160
#define LOGO_VIEW_WIDTH   60

//extern std::mutex g_get_image_mtx;
//extern std::condition_variable g_get_image_ok;

class CGlobalConfig
{
public:
    CGlobalConfig(void);
    virtual ~CGlobalConfig(void);
    int Init();
    void save();
    int load();
private:


public:

   volatile   int bRealTime; // 0 : 触发视频，1 实时视频
   int CheckExpireFlag;
   int end_face;
   int xz_dog ;
   int Roundness_Only;
   int timeset1;
   HTuple TCfg;
   HTuple  hv_CameraParam_BSF, hv_CamInBasePose_BSF;

   map<string, int > mapSerial2Index;
   map<int, string > mapIndex2Serial;

   map<int, string > mapIndex2comName;
   map<int, string > mapIndex2Spectum;


};
int GQueuePut(int id,void *pBuf, int size);
int GQueuePutL(int id,void *pBuf, int size);
int GQueueGet(int id,void *pBuf, unsigned bufsize,unsigned &size);
int GQueueGetL(int id,void *pBuf, unsigned bufsize,unsigned &size);

 void  GQueueInit();




extern CGlobalConfig  g_config;
extern float g_lookupTable[801][801];
extern CShmQueue g_queue;

