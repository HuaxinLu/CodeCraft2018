#ifndef DATA_PROCESSING_H_
#define DATA_PROCESSING_H_
#include <time.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

//虚拟机对象
struct Flavor
{
	int m_iCpuCount;//CPU数量
	int m_iMemCount;//MEM数量
	int m_iType;//虚拟机类型
};
//每条的训练数据
struct TrainData
{
	int m_iFlavorType;//虚拟机类型
	time_t m_tTime;//出现时间
};
//赛题信息
enum ObjectList
{
	CPU, MEM
};
struct ProblemInfo
{
	int m_iServerCpuCount;//服务器CPU数量
	int m_iServerMemCount;//服务器MEM数量
	int m_iServerDiskCount;//服务器硬盘数量
	int m_iFlavorCount;//需要预测的虚拟机数量
	ObjectList object;//优化目标
	time_t m_tStartTime;//开始时间
	time_t m_tEndTime;//结束时间
	int m_iDiffTIme;//间隔时间，以天为单位
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
	void ReadInputData();//读取题目要求信息
	void ReadTrainData();//读取需要预测的数据
	void SplitTrainData();//训练数据分单元
private:
	//读取信息用到的子函数
	time_t ReadTime(char* timeStr);
	Flavor ReadFlavor(char* flavorStr);
	//输入的文本信息数据
	char** trainData;
	int trainDataSize;
	char** inputData;
	int inputDataSize;
	//初步解析得到的数据
	ProblemInfo problemInfo;
	map<int, Flavor> flavorMap;//需要预测的虚拟机类型，映射对为type——flavor
	vector<TrainData> trainVec;//有用的训练数据条目
	vector<map<int, int>> trainDataUnit;//按len天划分单位得到的结果
};

#endif