#ifndef HEAD_H
#define HEAD_H

#include <cstdlib>//exit�ڴ�,rand
#include <cstdio>//�ļ����������ڴ�
#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>

using namespace std;

const int BMPHEADBYTES=1078;//256*256�Ҷ�ͼ����ļ�ͷ��������Ϣ�ľ���
const int BMPWIDTH=256;
const int BMPHEIGHT=256;
const int N=512;//�뱾��ģ
const int L=16;//ʸ��ά��
const double STEP=0.01;//ѧϰ����

void readBmp(const char * iBmpFile, unsigned char *BmpHeader, unsigned char **sourcePixel);
void saveBmp(const char *oBmpFile, unsigned char *BmpHeader, unsigned char **targetPixel);
int findMin(const double * d);



#endif