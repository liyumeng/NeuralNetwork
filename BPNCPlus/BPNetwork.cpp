#include "BPNetwork.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include "AnswerModel.h"
#include <algorithm>
#include<direct.h>
#include <string>

struct Xgreater
{
	bool operator()(const AnswerModel* lx, const AnswerModel* rx) const {
		return lx->Value > rx->Value;
	}
};

BPNetwork::BPNetwork()
{
}


BPNetwork::~BPNetwork()
{
	for (int i = 0; i < LayerCount; i++)
	{
		delete LayerList[i];
	}
	free(LayerList);
}


BPNetwork::BPNetwork(int layerCount, int * unitCountList, double alpha, double eta)
{
	srand(static_cast<unsigned>(time(nullptr)));	//��ʼ�����������

	Alpha = alpha;
	Eta = eta;
	LayerCount = layerCount;
	LayerList = static_cast<BPLayer**>(malloc(layerCount*sizeof(BPLayer*)));
	Times = 0;
	DropoutEnabled = 1;
	MaxTimes = INT32_MAX;
	//ʵ�ʲ��i��1��ʼ����������X��Ϊ��0������NextInput,��0��Ҳ���ؼ���
	//��һ�������ΪX���У�
	//WȨ��Ϊ��������һ���ز�֮���Ȩ��
	//��һ���Output�ǵ�һ�����δ����ֵ��NextInputΪ��һ����ļ���ֵ
	//���Ϊn�����磨���ﲻ������㣬����2�����磬����1������㣬1������㣩
	LayerList[0] = new BPLayer(0, unitCountList[0], this);//�ò��NextInputΪ�������X
	LayerList[1] = new BPLayer(unitCountList[0], unitCountList[1], this);
	for (int i = 2; i < layerCount; i++)
	{
		LayerList[i] = new BPLayer(unitCountList[i - 1], unitCountList[i], this, DropoutEnabled);
	}
	InputLayer = LayerList[1];//���𼤻��һ�����ֵ
	//����LayerList[1]->OutputΪ��һ����δ����ֵ
	//����LayerList[1]->NextInputΪ��һ���㼤��ֵ
	OutputLayer = LayerList[layerCount - 1];
}

double BPNetwork::Train(double* input, double* output)
{
	double e = 0;//���ۺ���ֵ

	for (int i = 0; i < LayerList[0]->OutputSize; i++)
	{
		LayerList[0]->NextInput[i] = input[i];
	}

	for (int i = 1; i < LayerCount; i++)
	{
		LayerList[i]->SetInput(LayerList[i - 1]->NextInput);
		LayerList[i]->Process();
		LayerList[i]->ActiveOutput();
	}

	//����
	for (int i = 0; i < OutputLayer->OutputSize; i++)
	{
		double x = OutputLayer->NextInput[i];	//ע���Ǽ����ֵ
		double y = output[i];
		OutputLayer->DeltaOutput[i] = x*(1 - x)*(x - y);
	}


	for (int c = LayerCount - 2; c > 0; c--)
	{
		BPLayer* curLayer = LayerList[c];
		BPLayer* nextLayer = LayerList[c + 1];

		//���㵱ǰ������Ľڵ�����DeltaOutput
		//֮ǰ�����ǹ�Ҫ��Ҫ���㳣����ֵ����ʵ�����ǲ��ã�Ҳû�취���㣬
		//��Ȼ������Weight[j][InputSize],��DeltaOutput��û�и�����׼��ֵ
		//����������������������������Ľڵ�������ֵ֮�����������������ֵ��
		for (int i = 0; i < curLayer->OutputSize; i++)
		{
			double x = curLayer->NextInput[i];	//ע�������Ǽ���ֵ
			double delta = 0;
			for (int j = 0; j < nextLayer->OutputSize; j++)
			{
				delta += nextLayer->DeltaOutput[j] * nextLayer->Weight[j][i];
			}
			//ע������ڵ�������Ҫ���ݵ���һ���еģ�������һ��ѭ���д��ݵģ�
			//��ʵ��������ڵ��Ѿ�ʧЧ�ˣ������������ҲΪ�㣬��Ӧ���ٴ����ˣ����Գ�����Dropout[i]��ֵ������
			curLayer->DeltaOutput[i] = x*(1 - x)*delta;
			if (curLayer->DropoutEnabled)
				curLayer->DeltaOutput[i] *= curLayer->Dropout[i];
		}
	}

	//����Ȩ�ؾ���(һ��Ҫ����������DeltaOutput��һ�����Weight����Ȼ�᲻����)
	for (int i = 1; i < LayerCount; i++)
	{
		LayerList[i]->UpdateWeight();
	}

	//�������
	for (int i = 0; i < OutputLayer->OutputSize; i++)
	{
		double x = OutputLayer->NextInput[i];	//ע���Ǽ����ֵ
		double y = output[i];
		e += 0.5*(x - y)*(x - y);
	}

	this->Times++;
	this->Error = e;
	return e;
}

void BPNetwork::TrainUntilConvergence(SampleLoader* loader)
{
	std::ofstream dst("�й�ģ����.csv");

	double e = 10000;
	double laste = 0;
	Times = 0;
	int sameCount = 0;
	int count = 0;
	double lastf = FastTest(loader->TestLoader);
	while (e > 1 && Times < this->MaxTimes&&count <= 50)
	{
		e = 0;
		for (int i = 0; i < loader->LineCount; i++)
		{
			e += this->Train(loader->Inputs[i], loader->Outputs[i]);
		}
		//std::cout << Times << ":" << e << "," << this->Alpha << std::endl;

		if (this->Alpha > 0.01)
			this->Alpha *= 0.96;

		if (abs(laste - e) < 0.05)	//5��������ͬ��������ѭ��
		{
			sameCount++;
			if (sameCount > 5) break;
		}
		else
			sameCount = 0;

		laste = e;
		double f = FastTest(loader->TestLoader);
		count++;

		dst << count << "," << e << "," << f << std::endl;
		std::cout << count << "," << e << "," << f << std::endl;

		std::string numStr = "dat/" + std::to_string(count);
		this->Save(numStr.c_str());
	}
	dst.close();
}

double* BPNetwork::Test(double* input)
{
	for (int i = 0; i < LayerList[0]->OutputSize; i++)
	{
		LayerList[0]->NextInput[i] = input[i];
	}

	for (int i = 1; i < LayerCount; i++)
	{
		LayerList[i]->SetInput(LayerList[i - 1]->NextInput);
		LayerList[i]->Process();
		LayerList[i]->ActiveOutput();
	}
	return OutputLayer->Output;
}

void BPNetwork::Test(SampleLoader* loader)
{
	int positiveAnswerCount = 389;
	int guessPositiveCount = std::min(400, loader->LineCount);
	int rightCount = 0;

	std::vector<AnswerModel*> items;

	for (int i = 0; i < loader->LineCount; i++)
	{
		this->Test(loader->Inputs[i]);
		items.push_back(new AnswerModel(loader->Items[i].UserId, loader->Items[i].ItemId, this->OutputLayer->Output[0], loader->Outputs[i][0]));
		if (i % 10000 == 0)
		{
			std::cout << i << "/" << loader->LineCount << std::endl;
		}
	}

	std::sort(items.begin(), items.end(), Xgreater());


	for (int i = 0; i < guessPositiveCount; i++)
	{
		if (items[i]->Y == 1)
		{
			rightCount++;
		}
	}

	double precision = 1.0*rightCount / positiveAnswerCount;
	double recall = 1.0*rightCount / guessPositiveCount;
	double f = 0;
	if (rightCount > 0)
		f = 2.0 * precision*recall / (precision + recall);
	std::cout << "��ȷ����:" << rightCount << std::endl;
	std::cout << "Ԥ��������:" << guessPositiveCount << std::endl;
	std::cout << "p:" << precision << std::endl;
	std::cout << "recall:" << recall << std::endl;
	std::cout << "f:" << f << std::endl;
}

//����Early Stopʱ���в��Ե�
double BPNetwork::FastTest(SampleLoader* loader)
{
	int rightCount = 0;
	int guessRightCount = 0;
	for (int i = 0; i < loader->LineCount; i++)
	{
		this->Test(loader->Inputs[i]);
		if (this->OutputLayer->Output[0] > 0)	//����������
		{
			guessRightCount++;
			if (loader->Outputs[i][0] > 0.5)	//��������
			{
				rightCount++;
			}
		}

	}
	return 2.0*rightCount / (guessRightCount + loader->PositiveCount);
}

void BPNetwork::Save(const char* dirname)
{
	char tmp[256];
	mkdir(dirname);
	for (int i = 0; i < LayerCount; i++)
	{
		sprintf(tmp, "%s/%d.csv", dirname, i);
		LayerList[i]->Save(tmp);
	}
	std::cout << "Ȩ�ؾ��������ϡ�" << std::endl;
}

void BPNetwork::Load(char* dirname)
{
	char tmp[256];
	for (int i = 0; i < LayerCount; i++)
	{
		sprintf(tmp, "%s/%d.csv", dirname, i);
		LayerList[i]->Load(tmp);
	}
	std::cout << "Ȩ�ؾ���������ϡ�" << std::endl;
}


//���Ԥ������������ļ���
void BPNetwork::Predict(SampleLoader* loader, char* filename)
{
	std::vector<AnswerModel*> items;

	for (int i = 0; i < loader->LineCount; i++)
	{
		this->Test(loader->Inputs[i]);
		items.push_back(new AnswerModel(loader->Items[i].UserId, loader->Items[i].ItemId, this->OutputLayer->Output[0]));
		//dst << loader->Items[i].UserId << "," << loader->Items[i].ItemId << "," << this->OutputLayer->Output[0] << "," << loader->Outputs[i][0] << std::endl;
		if (i % 10000 == 0)
		{
			std::cout << i << "/" << loader->LineCount << std::endl;
		}
	}

	std::sort(items.begin(), items.end(), Xgreater());

	std::ofstream dst(filename);

	for (int i = 0; i < 6000; i++)
	{
		dst << items[i]->UserId << "," << items[i]->ItemId << "," << items[i]->Value << std::endl;
	}
	dst.close();
	std::cout << "Ԥ�����������:" << filename << std::endl;
}

