#include "head.h"

int out[512];
void main()
{
	clock_t start, stop;
	start = clock();

	int i, j;

	//char * iBmpFile="J:\\NN class\\ͼ���ļ�\\LENA.BMP";
	char * iBmpFile = "LENA.BMP";
	//char * testBmpFile="C:\\NN class\\ͼ���ļ�\\CR.bmp";
	char * testBmpFile = "CR.BMP";
	char * oBmpFile = "REC_LENA.BMP";
	//char * oBmpFile="J:\\NN class\\ͼ���ļ�\\REC_LENA.BMP";
	char * DCT2DBMPFile = "DCT2DTransformCameraman.bmp";
	char * IDCT2DBMPFile = "IDCT2DTransformCameraman.bmp";

	unsigned char * BmpHeader = new unsigned char[BMPHEADBYTES];

	unsigned char ** sourcePixel;
	sourcePixel = new unsigned char*[BMPHEIGHT];
	for (i = 0; i < BMPHEIGHT; i++)
		sourcePixel[i] = new unsigned char[BMPWIDTH];
	unsigned char ** outPixel;
	outPixel = new unsigned char*[BMPHEIGHT];
	for (i = 0; i < BMPHEIGHT; i++)
		outPixel[i] = new unsigned char[BMPWIDTH];

	unsigned char ** vectorSample;
	vectorSample = new unsigned char*[BMPWIDTH*BMPHEIGHT / 16];
	for (i = 0; i < BMPWIDTH*BMPHEIGHT / 16; i++)
		vectorSample[i] = new unsigned char[L];
	unsigned char ** codeBook;
	codeBook = new unsigned char*[N];
	for (i = 0; i < N; i++)
		codeBook[i] = new unsigned char[L];

	readBmp(iBmpFile, BmpHeader, sourcePixel);//����ͷ�ļ�������
	//�γ�Ȩʸ��
	for (int y = 0; y < BMPHEIGHT; y++)
		for (int x = 0; x < BMPWIDTH; x++)
			vectorSample[y / 4 * 64 + x / 4][y % 4 * 4 + x % 4] = sourcePixel[y][x];

	/*	for (int y=0; y<BMPHEIGHT/4; y++)
		{
			for (int x=0; x<BMPWIDTH/4; x++)
			{
				//�γ�һ��ʸ������
				for (j=0; j<4; j++)
				{
					for (i=0; i<4; i++)
						vectorSample[x+y*BMPWIDTH/4][i+j*4]=sourcePixel[j+y*4][i+x*4];
				}
			}
		}
	*/

	//��ʼ��Ȩʸ��
	double ** W;
	W = new double*[N];
	for (i = 0; i < N; i++)
		W[i] = new double[L];
	srand((unsigned)time(NULL));
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < L; j++)
			W[i][j] = 255 * rand() / (RAND_MAX + 1.0);
	}

	double * d = new double[N];
	memset(d, 0, sizeof(double)*N);
	double * dtemp = new double[N];
	memset(dtemp, 0, sizeof(double)*N);
	double * wtemp = new double[L];
	memset(wtemp, 0, sizeof(double)*L);


	for (int k = 0; k < 100; k++)
	{
		int maxDis = 0;
		//ѵ������
		for (int c = 0; c < BMPWIDTH*BMPHEIGHT / 16; c++)
		{
			//�������
			for (j = 0; j < N; j++)
			{
				d[j] = 0;
				for (i = 0; i < L; i++)
					d[j] += (vectorSample[c][i] - W[j][i])*(vectorSample[c][i] - W[j][i]);
				d[j] = sqrt(d[j]);
			}
			int q = findMin(d);
			out[q] = 1;

			for (j = 0; j < N; j++)
			{
				dtemp[j] = 0;
				for (i = 0; i < L; i++)
					dtemp[j] += fabs(W[j][i] - W[q][i]);
			}

			for (j = 0; j < N; j++)
			{
				if (dtemp[j] < 100)
				{
					for (i = 0; i < L; i++)
					{
						W[j][i] += STEP*(vectorSample[c][i] - W[j][i]);
					}
				}
			}

			if (d[q] > maxDis)
				maxDis = d[q];
		}
		cout << maxDis << endl;

	}
	int useNum = 0;
	for (i = 0; i < N; i++)
		if (out[i] == 1)
			useNum++;
	cout << useNum << endl;


	//��������
	int * q = new int[BMPWIDTH*BMPHEIGHT / 16];
	for (int c = 0; c < BMPWIDTH*BMPHEIGHT / 16; c++)
	{
		//�������
		for (j = 0; j < N; j++)
		{
			for (i = 0; i < L; i++)
				d[j] += (vectorSample[c][i] - W[j][i])*(vectorSample[c][i] - W[j][i]);
			d[j] = sqrt(d[j]);
		}
		q[c] = findMin(d);
	}


	for (int c = 0; c < BMPWIDTH*BMPHEIGHT / 16; c++)
	{
		for (i = 0; i < L; i++)
			outPixel[c / 64 * 4 + i / 4][c % 64 * 4 + i % 4] = (int)W[q[c]][i];
	}



	saveBmp(oBmpFile, BmpHeader, outPixel);

	float mse = 0;
	for (int y = 0; y < BMPHEIGHT; y++)
		for (int x = 0; x < BMPWIDTH; x++)
			mse += (sourcePixel[y][x] - outPixel[y][x])*(sourcePixel[y][x] - outPixel[y][x]);
	mse = mse / BMPHEIGHT / BMPWIDTH;
	float psnr = 10 * log10(255.0 * 255 / mse);
	cout << psnr << endl;
	getchar();
	delete[]q;
	delete[]wtemp;
	delete[]dtemp;
	delete[]d;
	for (i = 0; i < N; i++)
		delete[]W[i];
	delete[]W;
	for (i = 0; i < N; i++)
		delete[]codeBook[i];
	delete[]codeBook;
	for (i = 0; i < BMPWIDTH*BMPHEIGHT / 16; i++)
		delete[]vectorSample[i];
	delete[]vectorSample;
	delete[]BmpHeader;
	for (i = 0; i < BMPHEIGHT; i++)
		delete[]outPixel[i];
	delete[]outPixel;
	for (i = 0; i < BMPHEIGHT; i++)
		delete[]sourcePixel[i];
	delete[]sourcePixel;

	stop = clock();
	cout << "cost " << stop - start << " ms" << endl;
}