#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include "DataProcessing.h"

//构造函数
DataProcessing::DataProcessing(char** _inputTrain, int _inputDataSize, char** _trainData, int _trainDataSize)
{
	trainData = _trainData;
	trainDataSize = _trainDataSize;
	inputData = _inputTrain;
	inputDataSize = _inputDataSize;
}

//读取输入信息
void DataProcessing::ReadInputData()
{
	//首先读取第0行的数据
	sscanf(inputData[0], "%d %d %d\n", &problemInfo.m_iServerCpuCount, &problemInfo.m_iServerMemCount, &problemInfo.m_iServerDiskCount);
	//读取第2行的数据
	problemInfo.m_iFlavorCount = atoi(inputData[2]);//需要预测的虚拟机数量
	//读取第3行开始的虚拟机信息
	for (int i = 0; i < problemInfo.m_iFlavorCount; i++)
	{
		Flavor flavorPredict = ReadFlavor(inputData[i + 3]);
		flavorMap.insert(pair<int, Flavor>(flavorPredict.m_iType, flavorPredict));
	}
	//读取优化目标 CPU or MEM
	problemInfo.object = strcmp(inputData[problemInfo.m_iFlavorCount + 4], "CPU\n") == 0 ? CPU : MEM;
	//读取时间
	problemInfo.m_tStartTime = ReadTime(inputData[problemInfo.m_iFlavorCount + 6]);
	problemInfo.m_tEndTime = ReadTime(inputData[problemInfo.m_iFlavorCount + 7]);
	problemInfo.m_iDiffTIme = difftime(problemInfo.m_tEndTime, problemInfo.m_tStartTime) / (60 * 60 * 24);
}
//读取训练数据
void DataProcessing::ReadTrainData()
{
	for (int i = 0; i < trainDataSize; i++)
	{
		TrainData trainDataStruct;
		tm timeStruct;
		char indexStr[16];
		sscanf(trainData[i], "%s\tflavor%d\t%d-%d-%d %d:%d:%d\n", indexStr, &trainDataStruct.m_iFlavorType, \
											&timeStruct.tm_year, &timeStruct.tm_mon, &timeStruct.tm_mday, \
											&timeStruct.tm_hour, &timeStruct.tm_min, &timeStruct.tm_sec);
		if (flavorMap.count(trainDataStruct.m_iFlavorType))//只寻找有用的训练数据
		{
			timeStruct.tm_year -= 1900;
			timeStruct.tm_mon -= 1;
			trainDataStruct.m_tTime = mktime(&timeStruct);
			trainVec.push_back(trainDataStruct);
		}
	}
}
//按照len长度划分训练数据单元
void DataProcessing::SplitTrainData()
{
	int len = problemInfo.m_iDiffTIme;//直接按照要求预测的时间长度为单位进行划分，这样只需要预测一步
	//首先计算训练数据跨度
	time_t trainStartTime = trainVec[0].m_tTime;//训练数据开始的时间
	int trainDiffTime = difftime(trainVec[trainVec.size() - 1].m_tTime, trainStartTime) / (60 * 60 * 24);
	int unitCount = (trainDiffTime + 1) / len;//总共分为多少单元
	int offset = (trainDiffTime + 1) % len;//为了保证最后一天的数据完整，舍弃掉最开始的一段数据
	int trainDataIndex = 0;//遍历训练数据的索引
	int trainUnitIndex = 0;//训练数据单元索引
	int trainDataRelativeTime = 0;//训练数据的相对间隔时间
	//为了保证最后一天的数据完整，舍弃掉最开始的一段数据
	while (trainDataRelativeTime < offset && trainDataIndex < trainVec.size())
		trainDataRelativeTime = difftime(trainVec[++trainDataIndex].m_tTime, trainStartTime) / (60 * 60 * 24);
	time_t trainOffsetTime = trainVec[++trainDataIndex].m_tTime;//相对时间计算基点
	trainDataRelativeTime = 0;
	//开始对数据进行按len大小分单元
	while (trainDataIndex < trainVec.size())
	{
		map<int, int> trainDataMap;//一个单元内flavor至个数的映射
		while (trainDataRelativeTime / len <= trainUnitIndex && trainDataIndex < trainVec.size())//数据还处于在一个单元内部
		{
			if (!trainDataMap.count(trainVec[trainDataIndex].m_iFlavorType))//如果没有存过
				trainDataMap.insert(pair<int, int>(trainVec[trainDataIndex].m_iFlavorType, 1));
			else
				trainDataMap[trainVec[trainDataIndex].m_iFlavorType]++;
			trainDataIndex++;
			if (trainDataIndex < trainVec.size())
				trainDataRelativeTime = difftime(trainVec[trainDataIndex].m_tTime, trainOffsetTime) / (60 * 60 * 24);
		}
		//如果单元内某个虚拟机没有出现过，要插入0
		map<int, Flavor>::iterator it = flavorMap.begin();
		for (; it != flavorMap.end(); it++)
		{
			if (!trainDataMap.count(it->first))//如果没有存过
				trainDataMap.insert(pair<int, int>(it->first, 0));
		}
		//跳出循环说明一个单元已经检测完毕
		trainDataUnit.push_back(trainDataMap);
		trainUnitIndex++;
	}
}

//功能函数，读取时间结构
time_t DataProcessing::ReadTime(char* timeStr)
{
	assert(timeStr != NULL);
	tm timeStruct;
	sscanf(timeStr, "%d-%d-%d %d:%d:%d\n", &timeStruct.tm_year, &timeStruct.tm_mon, &timeStruct.tm_mday, \
		&timeStruct.tm_hour, &timeStruct.tm_min, &timeStruct.tm_sec);
	timeStruct.tm_year -= 1900;
	timeStruct.tm_mon -= 1;
	return mktime(&timeStruct);
}
//功能函数，读取虚拟机结构
Flavor DataProcessing::ReadFlavor(char* flavorStr)
{
	Flavor flavorStruct;
	sscanf(flavorStr, "flavor%d %d %d\n", &flavorStruct.m_iType, &flavorStruct.m_iCpuCount, &flavorStruct.m_iMemCount);
	flavorStruct.m_iMemCount /= 1024;//换算为G
	return flavorStruct;
}