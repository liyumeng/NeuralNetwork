#ifndef HEAD_H
#define HEAD_H

#include <cstdlib>//exit在此,rand
#include <cstdio>//文件操作函数在此
#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>

using namespace std;

const int BMPHEADBYTES=1078;//256*256灰度图像从文件头到像素信息的距离
const int BMPWIDTH=256;
const int BMPHEIGHT=256;
const int N=512;//码本规模
const int L=16;//矢量维数
const double STEP=0.01;//学习步长

void readBmp(const char * iBmpFile, unsigned char *BmpHeader, unsigned char **sourcePixel);
void saveBmp(const char *oBmpFile, unsigned char *BmpHeader, unsigned char **targetPixel);
int findMin(const double * d);



#endif