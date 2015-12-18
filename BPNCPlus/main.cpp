#include<iostream>

#include "BPNetwork.h"
#include "SampleLoader.h"
#include <ctime>

using namespace std;

void Train()
{
	//��������
	//data�е�һ�б�ʾ��������n���ڶ���Ϊ����x��ά�ȣ�������Ϊ���y��ά��
	//�˺�n�У�Ϊx��y��ֵ����1 0 1��ʾ����x={1,0},���y={1}
	SampleLoader* loader = new SampleLoader("C:\\data\\small\\norm\\train1217.bin");
	loader->LoadBin();
	loader->EnableTestLoader();
	int inputSize = loader->InputSize;
	cout << "����ά��:" << inputSize << endl;
	cout << "��������:" << loader->LineCount << endl;

	//loader->EnableTestLoader();
	//�������磨1�����ز㣩��ÿ��Ľڵ����
	int unitCount[4] = { inputSize,500,loader->OutputSize };
	BPNetwork* net = new BPNetwork(3, unitCount, 0.4, 0.03);
	//net->Load("t");
	//ѵ��ֱ������
	net->TrainUntilConvergence(loader);
	net->Save("t");
	cout << "������" << net->Times << "��" << endl;
	printf("��ʼԤ��...\n");
	net->Test(loader->TestLoader);

	delete net;
}

void Test()
{
	//��������
	//data�е�һ�б�ʾ��������n���ڶ���Ϊ����x��ά�ȣ�������Ϊ���y��ά��
	//�˺�n�У�Ϊx��y��ֵ����1 0 1��ʾ����x={1,0},���y={1}
	cout << "����������..." << endl;
	SampleLoader* loader = new SampleLoader("C:\\data\\small\\norm\\test1218.bin");
	loader->LoadBin();
	int inputSize = loader->InputSize;
	cout << "����ά��:" << inputSize << endl;

	//�������磨1�����ز㣩��ÿ��Ľڵ����
	int unitCount[3] = { inputSize,500,loader->OutputSize };
	BPNetwork* net = new BPNetwork(3, unitCount);
	//ѵ��ֱ������
	net->Load("t");
	printf("��ʼԤ��...\n");
	//net->Predict(loader, "result.csv");
	net->Test(loader);
}

void TestLoadFile()
{
	clock_t start = clock();
	SampleLoader* loader = new SampleLoader("c:\\data\\small\\train1217.bin");
	loader->LoadBin();
	clock_t end = clock();
	cout << "��ʱ��" << (static_cast<double>(end - start) / CLOCKS_PER_SEC) << "s" << endl;
}

void main()
{
	Train();
	Test();
	getchar();
}