
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the SYUNEW2D_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
extern "C"
{
//��ȡ����ID
    int _stdcall GetID(  DWORD *ID_1,DWORD *ID_2,char *InPath);

//�����İ汾
    int _stdcall NT_GetIDVersion(int *Version,char *InPath);
//��ȡ������չ�汾
    int _stdcall NT_GetVersionEx(int *Version,char *InPath);

//����ָ���ļ�������ʹ����ͨ�㷨һ��
    int _stdcall FindPort_2(  int start,DWORD in_data,DWORD verf_data,char *OutPath);
//����ָ���ļ�������ʹ����ͨ�㷨����
    int _stdcall FindPort_3(  int start,DWORD in_data,DWORD verf_data,char *OutPath);
//���Ҽ�����
    int _stdcall FindPort(  int start,char *OutPath);

//�㷨����
    int _stdcall sWrite_2Ex_New(  DWORD in_data,DWORD *out_data,char *Path);
    int _stdcall sWriteEx_New(  DWORD in_data,DWORD *out_data,char *Path);
    int _stdcall sWrite_2Ex(  DWORD in_data,DWORD *out_data,char *Path);
    int _stdcall sWriteEx(  DWORD in_data,DWORD *out_data,char *Path);
    int _stdcall sRead(  DWORD *out_data,char *Path);
    int _stdcall sWrite(  DWORD in_data,char *Path);
    int _stdcall sWrite_2(  DWORD in_data,char *Path);

//дһ���ֽڵ���������
    int _stdcall YWriteEx(BYTE *InData,short address,short len,char *HKey,char *LKey,char *Path );
//дһ���ֽڵ��������У�һ�㲻ʹ��
    int __stdcall YWrite(BYTE InData,short address,char * HKey,char *LKey,char *Path );
//�Ӽ������ж�ȡһ���ֽ�
    int _stdcall YReadEx(BYTE *OutData,short address,short len,char *HKey,char *LKey,char *Path );
//�Ӽ������ж�ȡһ���ֽڣ�һ�㲻ʹ��
    int __stdcall YRead(BYTE *OutData,short address,char * HKey,char *LKey,char *Path );

//�Ӽ������ж��ַ���
    int __stdcall YReadString(char *string ,short Address,int len,char * HKey,char *LKey,char *Path );
//д�ַ�������������
    int __stdcall YWriteString(char *InString,short Address,char * HKey,char *LKey,char *Path );
//����д����
    int __stdcall SetWritePassword(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *InPath);
//���ö�����
    int __stdcall SetReadPassword(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *InPath);

//������ǿ�㷨��Կһ
    int __stdcall SetCal_2(char *Key,char *InPath);
//ʹ����ǿ�㷨һ���ַ������м���
    int _stdcall EncString(  char *InString,char *OutString,char *Path);
//ʹ����ǿ�㷨һ�Զ��������ݽ��м���
    int _stdcall Cal(  BYTE *InBuf,BYTE *OutBuf,char *Path);

//������ǿ�㷨��Կ��
    int __stdcall SetCal_New(char *Key,char *InPath);
//ʹ����ǿ�㷨�����ַ������м���
    int _stdcall Cal_New(  BYTE *InBuf,BYTE *OutBuf,char *Path);
//ʹ����ǿ�㷨�����ַ������м���
    int _stdcall EncString_New(  char *InString,char *OutString,char *Path);

//ʹ����ǿ�㷨���ַ������н���(ʹ�����)
    void _stdcall DecString(  char *InString,char *OutString,char *Key);
//ʹ����ǿ�㷨�Զ��������ݽ��м���(ʹ�����)
    void _stdcall  DecBySoft(    BYTE  *   aData,   BYTE   *   aKey   )  ;
    void _stdcall  EncBySoft(   BYTE  *   aData,  BYTE   *   aKey   )   ;
//�ַ��������������ݵ�ת��
    void __stdcall  HexStringToByteArray(char * InString,BYTE *out_data);
    void __stdcall  ByteArrayToHexString(BYTE *in_data,char * OutString,int len);
//��ʼ��������,ע�⣬��ʼ���������е�����Ϊ0����д����ҲΪ00000000-00000000����ǿ�㷨���ᱻ��ʼ��
    int __stdcall ReSet(char *InPath);
}
