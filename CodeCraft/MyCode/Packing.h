#ifndef PACKING_H_
#define PACKING_H_

#include <time.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>
using namespace std;

//��¼�������������ʣ������
struct FlavorPredict 
{
	int m_iCpuCount;//CPU����
	int m_iMemCount;//MEM����
	int m_iType;//���������
	bool m_bUsed;//�Ƿ�ʹ����
};

//��¼ʹ�ù��ķ�����
struct ServerUsed
{
	map<int, int> flavorInside;//�������ڵ�װ����Ϣ
};

class Predict;
class DataProcessing;

class Packing
{
public:
	Packing(class DataProcessing &dp, class Predict &pr);
	void PackingStart();
	string PrintResult();
private:
	void PackOneFlavor(ServerUsed &su, int flavorType);
	vector<FlavorPredict> flavorPredictVec;
	vector<ServerUsed> serverUsedVec;
	int m_iServerCpuCount;//������CPU����
	int m_iServerMemCount;//������MEM����
	int m_iFlavorNumber;//Ԥ��������������
	int m_iRestFlavor;//ʣ������������
	int m_iFlavorTypes;//�����������
	string m_sPackingResult;
};

#endif