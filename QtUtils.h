#pragma once
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <iostream>
#include<windows.h>
#include<QStringList>




int GetAllFilePath(const std::string& dir, std::vector<std::string>& filenames, QStringList filters=QStringList()<<"*.jpg"<<"*.png");
int GetAllFilePath(QString & dir, QStringList & filenames, QStringList filters=QStringList()<<"*.jpg"<<"*.png");

std::string  itoa(int i);
int GetAllDirs( const std::string& rootPath,std::vector<std::string> & dirs );

//ɾ��Ŀ¼��ֱ�����̿ռ���Сֵ����miniSize,����Σ��
bool RemoveDirsWhenNotEnough(char * rootdir, int miniSize);

QString D2Str(double f, int Precesion=3);
std::string Double2String(double dValue,int precision);


