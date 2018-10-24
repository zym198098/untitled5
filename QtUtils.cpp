#include "QtUtils.h"
#include <vector>
#include<direct.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <stdio.h>

#include <iostream>
#include <iomanip> //要用到格式控制符
#include <QDir>
#include<sstream>

#include <direct.h>
#include <io.h>
#include <fcntl.h>

#include"Utils.h"
using namespace std;





int GetAllFilePath(const std::string& rootdir, std::vector<std::string>& filenames, QStringList filters)
{

//	QStringList filters;
//	filters << "*.jpg" << "*.png" << "*.gif";

    QDir dir(QString::fromStdString(rootdir));
    if (!dir.exists())
        return -1;

    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst);
//    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    int iListSize=list.size();
    int i = 0;
    bool bIsDir;
    while (i<list.size())
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="."|fileInfo.fileName()=="..")
        {
            i++;
            continue;
        }
        bIsDir = fileInfo.isDir();
        if (bIsDir)
        {
            GetAllFilePath(fileInfo.filePath().toStdString(), filenames,filters);
        }
        else
        {

            filenames.push_back(fileInfo.absoluteFilePath().toStdString());
        }
        i++;
    }

    return 0;
}



int GetAllFilePath(QString & rootdir, QStringList & filenames, QStringList filters)
{



    QDir dir(rootdir);
    if (!dir.exists())
        return -1;

    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst);
//    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    int iListSize=list.size();
    int i = 0;
    bool bIsDir;
    while (i<list.size())
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="."|fileInfo.fileName()=="..")
        {
            i++;
            continue;
        }
        bIsDir = fileInfo.isDir();
        if (bIsDir)
        {
          // GetAllFilePath(fileInfo.filePath(), filenames,filters);
			
			
        }
        else
        {

            filenames.push_back(fileInfo.absoluteFilePath());
        }
        i++;
    }

    return 0;
}

std::string  itoa(int i)
{
    return QString::number(i,10).toStdString();
}



//to do ,make filter ok
int GetAllDirs( const std::string& rootPath,std::vector<std::string> & dirs )
{

    QDir dir(QString::fromStdString(rootPath));
    if (!dir.exists())
        return -1;

    dir.setFilter(QDir::Dirs);
    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    bool bIsDir;
    do
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="."|fileInfo.fileName()=="..")
        {
            i++;
            continue;
        }
        dirs.push_back(fileInfo.filePath().toStdString());
        i++;
    }
    while(i<list.size());
    return 0;

}




bool RemoveDirsWhenNotEnough(char * rootdir, int miniSize)
{
    float total,  free;

    if(!GetDiskFreeSpaceEx(rootdir, total,  free))
    {
        return false;
    }

    if(free<miniSize)
    {
        //remove some dirs
        vector<std::string> v_dirs;
        GetAllDirs(string(rootdir),v_dirs);

        for(int j=0; j<v_dirs.size() && free<miniSize; j++)
        {
            string dir=v_dirs[j];
            QDir qdir(QString::fromStdString(dir));
            qdir.removeRecursively();
            if(!GetDiskFreeSpaceEx(rootdir, total,  free))
            {
                return false;
            }
        }

    }
    return true;
}

bool CheckProcExist(void **pLock)
{
    wchar_t wPath[256];
    string path=GetCurrentProcessName();
    QFileInfo finfo(QString::fromStdString(path));
    string filename= finfo.fileName().toStdString();
    string lockfile="C:\\windows\\" + filename+ ".lock";


    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  c2w(wPath,sizeof(wPath),lockfile.c_str()));

    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)
    {
        CloseHandle(m_hMutex);
        m_hMutex  =  NULL;
        return  true;
    }


    return false;
}

QString D2Str(double f, int Precesion)
{
    int iRound = (int)f;
    int k = 0;
    //计算小树部分是多少位有效数字
    while (iRound)
    {
        k++;
        iRound /= 10;
    }
    return QString::number(f, 'g', Precesion + k);
}
std::string Double2String(double dValue,int precision)
{
    return QString::number(dValue,'g',precision).toStdString();
}


