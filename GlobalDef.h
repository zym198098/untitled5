#pragma once
#include <string>
#include <vector>


typedef struct
{
    int seq;
    bool bNeedProcess;
    bool bNeedStorage;
    bool bNeedDisplay;

    float x_dis;
    float y_dis;
    float pix_dist;

    time_t wati_begin_time;

    int iCamid;
    int  iOk; //image_process result
    int  t;
    int flag; //pass from session buf
    time_t t_sync;
} S_result;


#define CAM_PIPE_SHM_ID_BASE  100

#define IMG_PIPE_SHM_ID_BASE  200

#define DISP_PIPE_SHM_ID_BASE  300

#define UI_PIPE_SHM_ID_BASE   400

#define CAM_CTRL_PIPE_SHM_ID_BASE 500
#define IMG_CTRL_PIPE_SHM_ID_BASE 550
#define CAM_CTRL_FEED_BACK 700


//#define CAMERA_NUM 1

#define SESSION_BUF_NUM 60

//#define DISPLAY_SESSION_NUM 21

enum
{
	CAM_SRC_DH = 0,
    //CAM_SRC_DH,
    //CAM_SRC_BASLER,
    //CAM_SRC_GIA,
    CAM_SRC_FILE,
    CAM_TYPE_MAX
};


typedef struct
{
    int run_mode;
    int input_source;
    volatile bool quit;
    int iDebugSrc; //0: 文件，1 数据库输入,2 其他
} S_SysCtrl;


typedef struct _statics
{
    int iNgCnt;
    int iOkCnt;
    _statics()
    {
        iNgCnt=0;
        iOkCnt=0;
    }
} S_Statics;
extern S_Statics  g_statics;
extern S_SysCtrl g_SysCtrl;
