#include "CwxThreadWithQueueBuf.h"
CwxThreadWithQueueBuf::CwxThreadWithQueueBuf():m_bQuit(false)
{
    m_iProductNum=0;
    m_iConSumerNum=0;
    m_iProductKey=0;
    m_iConSumerKey=0;
    m_iProductShmSize=0;
    m_iConSumerShmSize=0;
    memset(m_DataProducer,0,sizeof(m_DataProducer));
    memset(m_DataConsumer,0,sizeof(m_DataConsumer));

}

CwxThreadWithQueueBuf::CwxThreadWithQueueBuf(CShmProducer * productor, CShmComsumer * Consumer):m_bQuit(false)
{
    //m_pInputQueue=NULL;
    // m_pOutPutQueue=NULL;
    m_iProductNum=1;
    m_iConSumerNum=1;
    m_iProductKey=productor->mq_->GetKey();
    m_iConSumerKey=Consumer->mq_->GetKey();
    m_iProductShmSize=productor->mq_->GetSize();;
    m_iConSumerShmSize=Consumer->mq_->GetSize();
    memset(m_DataProducer,0,sizeof(m_DataProducer));
    memset(m_DataConsumer,0,sizeof(m_DataConsumer));
    m_DataProducer[0]=productor;
    m_DataConsumer[0]=Consumer;
}

int CwxThreadWithQueueBuf::ShmInit(int  iProductId, int iConsumerId,bool bClear,int iProductNum,int iConsumerNum,bool bProductLock,bool bConsummerLock, int iProductShmSize,int iConsumerSize)
{
    CShmProducer * productor;
    CShmComsumer * Consumer;
    memset(m_DataProducer,0,sizeof(m_DataProducer));
    memset(m_DataConsumer,0,sizeof(m_DataConsumer));
    for(int i=0; i<iProductNum; i++)
    {
        if(bProductLock)
            productor=new CShmProducerL();
        else
            productor=new CShmProducer();
        if(productor->init(iProductId+i, iProductShmSize)<0)
        {
            return -1;
        }
	if(bClear)
	{
	 productor->clear();
	}
        m_DataProducer[i]=productor;
    }
    for(int i=0; i<iConsumerNum; i++)
    {
        if(bConsummerLock)
            Consumer=new CShmComsumerL();
        else
            Consumer=new CShmComsumer();
	if(bClear)
	{
		Consumer->clear();
	}
        if( Consumer->init(iConsumerId+i,iConsumerSize)<0)
        {
            return -2;
        }
        m_DataConsumer[i]=Consumer;
    }

    m_iProductNum=iProductNum;
    m_iConSumerNum=iConsumerNum;
    m_iProductKey=iProductId;
    m_iConSumerKey=iConsumerId;
    m_iProductShmSize=iProductShmSize;
    m_iConSumerShmSize=iConsumerSize;

    return 0;
}

CwxThreadWithQueueBuf::~CwxThreadWithQueueBuf(void)
{
}

int  CwxThreadWithQueueBuf::GetFromInputQueue(char **p,int & per,int id)
{
    char buf[2048];
    unsigned int data_len;
    unsigned int flow;
    if(m_DataConsumer[id]==NULL) return -100;
    if(m_DataConsumer[id]->comsume(buf, sizeof(buf), data_len, flow)>=0)
    {
        if(data_len!=sizeof(char *))
        {
            return -1;
        }
        *p=  *(char **)buf;
        per=m_DataConsumer[id]->mq_->GetMsgCount();
        return 0;
    }
    return -1;
}


int CwxThreadWithQueueBuf::PutToOutPutQueue(char *p,int & per,int id)
{
    int iRet;

    if(m_DataProducer[id]==NULL) return -100;

    if((iRet=m_DataProducer[id]->produce((void *)&p, sizeof(char *),  0))<0)
    {
        return iRet;
    }
    per=iRet;
    return 0;
}


//void CwxThreadWithQueueBuf::SetBufQueue(CBufIdQueue * pInPutQue,CBufIdQueue * pOutPutQueue)
//{
// m_pInputQueue=pInPutQue;
// m_pOutPutQueue=pOutPutQueue;
//}

void CwxThreadWithQueueBuf::Execute()
{
    if(!this->isRunning())
    {
        start();
    }
}

void* CwxThreadWithQueueBuf::Entry()
{
    return NULL;
}

void CwxThreadWithQueueBuf::run()
{
    Entry();
}
