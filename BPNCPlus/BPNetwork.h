#pragma once
#include"BPLayer.h"
#include "SampleLoader.h"

class BPNetwork
{
public:
	BPNetwork();
	~BPNetwork();

	//�����layerCountҪ���������
	BPNetwork(int layerCount, int* unitCountList, double alpha = 0.5, double eta = 0.05);

	//ѵ������
	double Train(double* input, double* output);

	//����
	double* Test(double* input);
	void Test(SampleLoader* loader);
	double FastTest(SampleLoader* loader);
	void Save(const char* dirname);
	void Load(char* dirname);
	void Predict(SampleLoader* loader, char* filename);
	//ѵ��ֱ������
	void TrainUntilConvergence(SampleLoader* loader);

	BPLayer** LayerList;

	//����������ʵ������ X����->W1Ȩ��->��һ���� �γɵ�����
	//��������InputLayer=LayerList[1];��ΪLayerList[0]ʵ��ûʲô���塣
	//����InputLayer->Input����X����
	BPLayer* InputLayer;
	//���������������һ������->WnȨ��->����� �γɵ�����
	//��������OutputLayer->Output�������յ����
	BPLayer* OutputLayer;

	int LayerCount;	//�������

	double Eta = 0.001;		//��һ��
	double Alpha;		//����ѧϰ��
	double Error;	//������

	long long Times = 0;	//��������
	long long MaxTimes;
	int DropoutEnabled;
};

