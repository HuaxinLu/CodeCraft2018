#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include "DataProcessing.h"

//���캯��
DataProcessing::DataProcessing(char** _inputTrain, int _inputDataSize, char** _trainData, int _trainDataSize)
{
	trainData = _trainData;
	trainDataSize = _trainDataSize;
	inputData = _inputTrain;
	inputDataSize = _inputDataSize;
}

//��ȡ������Ϣ
void DataProcessing::ReadInputData()
{
	//���ȶ�ȡ��0�е�����
	sscanf(inputData[0], "%d %d %d\n", &problemInfo.m_iServerCpuCount, &problemInfo.m_iServerMemCount, &problemInfo.m_iServerDiskCount);
	//��ȡ��2�е�����
	problemInfo.m_iFlavorCount = atoi(inputData[2]);//��ҪԤ������������
	//��ȡ��3�п�ʼ���������Ϣ
	for (int i = 0; i < problemInfo.m_iFlavorCount; i++)
	{
		Flavor flavorPredict = ReadFlavor(inputData[i + 3]);
		flavorMap.insert(pair<int, Flavor>(flavorPredict.m_iType, flavorPredict));
	}
	//��ȡ�Ż�Ŀ�� CPU or MEM
	problemInfo.object = strcmp(inputData[problemInfo.m_iFlavorCount + 4], "CPU\n") == 0 ? CPU : MEM;
	//��ȡʱ��
	problemInfo.m_tStartTime = ReadTime(inputData[problemInfo.m_iFlavorCount + 6]);
	problemInfo.m_tEndTime = ReadTime(inputData[problemInfo.m_iFlavorCount + 7]);
	problemInfo.m_iDiffTIme = difftime(problemInfo.m_tEndTime, problemInfo.m_tStartTime) / (60 * 60 * 24);
}
//��ȡѵ������
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
		if (flavorMap.count(trainDataStruct.m_iFlavorType))//ֻѰ�����õ�ѵ������
		{
			timeStruct.tm_year -= 1900;
			timeStruct.tm_mon -= 1;
			trainDataStruct.m_tTime = mktime(&timeStruct);
			trainVec.push_back(trainDataStruct);
		}
	}
}
//����len���Ȼ���ѵ�����ݵ�Ԫ
void DataProcessing::SplitTrainData()
{
	int len = problemInfo.m_iDiffTIme;//ֱ�Ӱ���Ҫ��Ԥ���ʱ�䳤��Ϊ��λ���л��֣�����ֻ��ҪԤ��һ��
	//���ȼ���ѵ�����ݿ��
	time_t trainStartTime = trainVec[0].m_tTime;//ѵ�����ݿ�ʼ��ʱ��
	int trainDiffTime = difftime(trainVec[trainVec.size() - 1].m_tTime, trainStartTime) / (60 * 60 * 24);
	int unitCount = (trainDiffTime + 1) / len;//�ܹ���Ϊ���ٵ�Ԫ
	int offset = (trainDiffTime + 1) % len;//Ϊ�˱�֤���һ��������������������ʼ��һ������
	int trainDataIndex = 0;//����ѵ�����ݵ�����
	int trainUnitIndex = 0;//ѵ�����ݵ�Ԫ����
	int trainDataRelativeTime = 0;//ѵ�����ݵ���Լ��ʱ��
	//Ϊ�˱�֤���һ��������������������ʼ��һ������
	while (trainDataRelativeTime < offset && trainDataIndex < trainVec.size())
		trainDataRelativeTime = difftime(trainVec[++trainDataIndex].m_tTime, trainStartTime) / (60 * 60 * 24);
	time_t trainOffsetTime = trainVec[++trainDataIndex].m_tTime;//���ʱ��������
	trainDataRelativeTime = 0;
	//��ʼ�����ݽ��а�len��С�ֵ�Ԫ
	while (trainDataIndex < trainVec.size())
	{
		map<int, int> trainDataMap;//һ����Ԫ��flavor��������ӳ��
		while (trainDataRelativeTime / len <= trainUnitIndex && trainDataIndex < trainVec.size())//���ݻ�������һ����Ԫ�ڲ�
		{
			if (!trainDataMap.count(trainVec[trainDataIndex].m_iFlavorType))//���û�д��
				trainDataMap.insert(pair<int, int>(trainVec[trainDataIndex].m_iFlavorType, 1));
			else
				trainDataMap[trainVec[trainDataIndex].m_iFlavorType]++;
			trainDataIndex++;
			if (trainDataIndex < trainVec.size())
				trainDataRelativeTime = difftime(trainVec[trainDataIndex].m_tTime, trainOffsetTime) / (60 * 60 * 24);
		}
		//�����Ԫ��ĳ�������û�г��ֹ���Ҫ����0
		map<int, Flavor>::iterator it = flavorMap.begin();
		for (; it != flavorMap.end(); it++)
		{
			if (!trainDataMap.count(it->first))//���û�д��
				trainDataMap.insert(pair<int, int>(it->first, 0));
		}
		//����ѭ��˵��һ����Ԫ�Ѿ�������
		trainDataUnit.push_back(trainDataMap);
		trainUnitIndex++;
	}
}

//���ܺ�������ȡʱ��ṹ
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
//���ܺ�������ȡ������ṹ
Flavor DataProcessing::ReadFlavor(char* flavorStr)
{
	Flavor flavorStruct;
	sscanf(flavorStr, "flavor%d %d %d\n", &flavorStruct.m_iType, &flavorStruct.m_iCpuCount, &flavorStruct.m_iMemCount);
	flavorStruct.m_iMemCount /= 1024;//����ΪG
	return flavorStruct;
}