#ifndef DATA_PROCESSING_H_
#define DATA_PROCESSING_H_
#include <time.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

//���������
struct Flavor
{
	int m_iCpuCount;//CPU����
	int m_iMemCount;//MEM����
	int m_iType;//���������
};
//ÿ����ѵ������
struct TrainData
{
	int m_iFlavorType;//���������
	time_t m_tTime;//����ʱ��
};
//������Ϣ
enum ObjectList
{
	CPU, MEM
};
struct ProblemInfo
{
	int m_iServerCpuCount;//������CPU����
	int m_iServerMemCount;//������MEM����
	int m_iServerDiskCount;//������Ӳ������
	int m_iFlavorCount;//��ҪԤ������������
	ObjectList object;//�Ż�Ŀ��
	time_t m_tStartTime;//��ʼʱ��
	time_t m_tEndTime;//����ʱ��
	int m_iDiffTIme;//���ʱ�䣬����Ϊ��λ
};

class Predict;
class Packing;

class DataProcessing
{
public:
	friend class Predict;
	friend class Packing;

	DataProcessing(char** _inputTrain, int _trainDataSize, char** _inputData, int _inputDataSize);
	//~DataProcessing(){};
	void ReadInputData();//��ȡ��ĿҪ����Ϣ
	void ReadTrainData();//��ȡ��ҪԤ�������
	void SplitTrainData();//ѵ�����ݷֵ�Ԫ
private:
	//��ȡ��Ϣ�õ����Ӻ���
	time_t ReadTime(char* timeStr);
	Flavor ReadFlavor(char* flavorStr);
	//������ı���Ϣ����
	char** trainData;
	int trainDataSize;
	char** inputData;
	int inputDataSize;
	//���������õ�������
	ProblemInfo problemInfo;
	map<int, Flavor> flavorMap;//��ҪԤ�����������ͣ�ӳ���Ϊtype����flavor
	vector<TrainData> trainVec;//���õ�ѵ��������Ŀ
	vector<map<int, int>> trainDataUnit;//��len�컮�ֵ�λ�õ��Ľ��
};

#endif