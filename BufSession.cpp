#include "BufSession.h"


CBufSession::CBufSession(void)
{
    Clear();
    GenEmptyObj(&m_Image);
    TupleGenConst(100, 0, &Flag); //��������
}


CBufSession::~CBufSession(void)
{
}




