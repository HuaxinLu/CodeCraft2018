#ifndef PACKING_H_
#define PACKING_H_

#include <time.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>
using namespace std;

//记录虚拟机的类型与剩余数量
struct FlavorPredict 
{
	int m_iCpuCount;//CPU数量
	int m_iMemCount;//MEM数量
	int m_iType;//虚拟机类型
	bool m_bUsed;//是否被使用了
};

//记录使用过的服务器
struct ServerUsed
{
	map<int, int> flavorInside;//服务器内的装箱信息
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
	int m_iServerCpuCount;//服务器CPU数量
	int m_iServerMemCount;//服务器MEM数量
	int m_iFlavorNumber;//预测出的虚拟机总数
	int m_iRestFlavor;//剩余的虚拟机数量
	int m_iFlavorTypes;//虚拟机类型数
	string m_sPackingResult;
};

#endif