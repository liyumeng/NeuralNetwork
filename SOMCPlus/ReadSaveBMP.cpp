#include "head.h"

void readBmp(const char * iBmpFile, unsigned char *BmpHeader, unsigned char **sourcePixel)
{

	FILE * fp = fopen(iBmpFile, "rb");

	if (fp == NULL)
	{
		cerr << "cannot open " << iBmpFile << endl;
		exit(1);
	}

	fread(BmpHeader, 1, BMPHEADBYTES, fp);//��fp�ļ��ж�ȡ�ļ�ͷ����Ϣͷ��BmpHeader
	for (int i = 0; i < BMPHEIGHT; i++)
	{
		fread(sourcePixel[i], 1, BMPWIDTH, fp);//��fp�ļ��ж�ȡ�������ݵ�sourcePixel
	}

	fclose(fp);
}

void saveBmp(const char *oBmpFile, unsigned char *BmpHeader, unsigned char **targetPixel)
{

	FILE *fp;

	fp = fopen(oBmpFile, "wb");
	if (fp == NULL)
	{
		cout << "out bmpfile open error!" << endl;
		exit(1);
	}

	fwrite(BmpHeader, 1, BMPHEADBYTES, fp);//��BmpHeaderд���ļ�ͷ����Ϣͷ���ļ�fp
	for (int i = 0; i < BMPHEIGHT; i++)
	{
		fwrite(targetPixel[i], 1, BMPWIDTH, fp);//��targetPixelд���������ݵ��ļ�fp
	}
	fclose(fp);
}

int findMin(const double * d)
{
	double temp = d[0];
	int q = 0;
	for (int i = 0; i < N; i++)
	{
		if (d[i] < temp)
		{
			temp = d[i];
			q = i;
		}
	}
	return q;
}


