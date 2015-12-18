#pragma once

class BPNetwork;

/*
������Ĳ����
ʹ��˵��
1. ʵ��������	inputSize	:	����ڵ�ĸ���
				outputSize	:	����ڵ�ĸ���
2. ����	SetInput	��������ڵ��ֵ
3. ���� Train		����δ��������,�õ�Output
4. ���� ActiveOutput	����������õ���һ�������ڵ�NextInput
*/
class BPLayer
{

public:
	BPLayer();
	~BPLayer();

	BPLayer(int inputSize, int outputSize, BPNetwork* net, int dropoutEnabled = 0);

	//1.��������
	void SetInput(double* input);

	//2.�����������
	double* Process(int isTesting = 0);
	double* TestingProcess();
	//3.���������ֵ
	double* ActiveOutput();

	//4.����Ȩ������
	void UpdateWeight();
	void Save(char* filename);
	void Load(char* filename);
	//���������ڵ�
	//��Ҫ����SetInput��������ֵ
	double* Input;

	//δ����ı������
	//��Ҫ����Process()���ܵõ���ֵ
	double*	Output;

	//��������һ������
	//��Ҫ����ActiveOutput()���ܵõ���ֵ
	double* NextInput = nullptr;

	//��ʼΪ���Ȩ��
	double** Weight;

	//�ϴ�Ȩ�صĸ���ֵ
	double** DeltaWeight;
	double** LastDeltaWeight;

	//�����������ڵ�����ͬ
	double* DeltaOutput;

	int InputSize;
	int OutputSize;

	int BatchSize;

	BPNetwork* Net = nullptr;
	double m_dropoutFraction;
	double* Dropout;
	int DropoutEnabled;

private:
	double** m_g2;
	double** m_u2;
	double m_rho;
	double m_epsilon;
	int m_indexInBatch;
};

