#include "head.h"

int out[512];
void main()
{
	clock_t start, stop;
	start = clock();

	int i, j;

	//char * iBmpFile="J:\\NN class\\图像文件\\LENA.BMP";
	char * iBmpFile = "LENA.BMP";
	//char * testBmpFile="C:\\NN class\\图像文件\\CR.bmp";
	char * testBmpFile = "CR.BMP";
	char * oBmpFile = "REC_LENA.BMP";
	//char * oBmpFile="J:\\NN class\\图像文件\\REC_LENA.BMP";
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

	readBmp(iBmpFile, BmpHeader, sourcePixel);//读出头文件和像素
	//形成权矢量
	for (int y = 0; y < BMPHEIGHT; y++)
		for (int x = 0; x < BMPWIDTH; x++)
			vectorSample[y / 4 * 64 + x / 4][y % 4 * 4 + x % 4] = sourcePixel[y][x];

	/*	for (int y=0; y<BMPHEIGHT/4; y++)
		{
			for (int x=0; x<BMPWIDTH/4; x++)
			{
				//形成一个矢量样本
				for (j=0; j<4; j++)
				{
					for (i=0; i<4; i++)
						vectorSample[x+y*BMPWIDTH/4][i+j*4]=sourcePixel[j+y*4][i+x*4];
				}
			}
		}
	*/

	//初始化权矢量
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

	double step = 0.01;
	for (int k = 0; k < 1; k++)
	{
		int maxDis = 0;
		//训练网络
		for (int c = 0; c < BMPWIDTH*BMPHEIGHT / 16; c++)
		{
			//计算距离
			for (j = 0; j < N; j++)
			{
				d[j] = 0;
				for (i = 0; i < L; i++)
					d[j] += (vectorSample[c][i] - W[j][i])*(vectorSample[c][i] - W[j][i]);
			}
			int q = findMin(d);
			out[q] = 1;

			//更新权重矢量
			for (j = 0; j < N; j++)
			{
				if (abs(j - q) < 3)
				{
					for (i = 0; i < L; i++)
					{
						W[j][i] += step*(vectorSample[c][i] - W[j][i]);
					}
				}
			}

			if (d[q] > maxDis)
				maxDis = d[q];

		}
		if (step > 0.003)
			step = step*0.96;
		cout << maxDis << "," << step << endl;

	}
	int useNum = 0;
	for (i = 0; i < N; i++)
		if (out[i] == 1)
			useNum++;
	cout << useNum << endl;


	//测试网络
	int * q = new int[BMPWIDTH*BMPHEIGHT / 16];
	for (int c = 0; c < BMPWIDTH*BMPHEIGHT / 16; c++)
	{
		//计算距离
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
