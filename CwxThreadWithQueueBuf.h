#pragma once

#include <QThread>

#include"tshmcommu.h"

//#define SESSION_BUF_NUM 30
class CwxThreadWithQueueBuf :public QThread
{
public:
    CwxThreadWithQueueBuf();
    CwxThreadWithQueueBuf(CShmProducer * product, CShmComsumer * Consumer);
    int  ShmInit(int  iProductId, int iConsumerId,bool bClear=true,int iProductNum=1,int iConsumerNum=1,bool bProductLock=false,bool bConsummerLock=false, int iProductShmSize=4096,int iConsumerSize=4096);

    virtual ~CwxThreadWithQueueBuf(void);
//   void SetBufQueue(CBufIdQueue * pInPutQue,CBufIdQueue * pOutPutQueue);
    virtual void* Entry();
    void Execute();

    void run();
    void Delete()
    {
        m_bQuit=true;
    }
    bool TestDestroy()
    {
        return m_bQuit;
    }
    int  GetFromInputQueue(char **p,int & per,int id=0);
    


    int PutToOutPutQueue(char *p,int & per,int id=0);
    


protected:
    CShmProducer  * m_DataProducer[10];
    CShmComsumer   * m_DataConsumer[10];
    int m_iProductNum;
     int m_iConSumerNum;
     int m_iProductKey;
      int m_iConSumerKey;
	int m_iProductShmSize;
	int m_iConSumerShmSize;
//   CBufIdQueue *m_pInputQueue;
    //  CBufIdQueue *m_pOutPutQueue;
private :
    bool m_bQuit;
};


