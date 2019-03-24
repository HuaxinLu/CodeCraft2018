#include "MyPredict.h"
#include "DataProcessing.h"

Predict::Predict()
{
	m_iAlphaS1 = 0.3;//һ��Ȩ��
	m_iAlphaS2 = 0.7;//����Ȩ��
	m_iAlphaS3 = 0.9;//����Ȩ��
	m_iPredictSum = 0;//Ԥ��������������
}
//��ʼ����Ԥ��
void Predict::PredictStart(class DataProcessing &dp, enum PredictAlgorithm pa)
{
	//Ѱ��Ҫ��Ԥ������������
	map<int, Flavor>::iterator it = dp.flavorMap.begin();
	for (; it != dp.flavorMap.end(); it++)
	{
		//�ֱ��ÿһ���������������
		vector<int> historyData;
		for (int i = 0; i < dp.trainDataUnit.size(); i++)
			historyData.push_back(dp.trainDataUnit[i][it->first]);
		if (it->first == 14)
		{
			volatile int re = it->first;
		}
		//����Ԥ��
		int result = 0;
		if (pa == SingleExponentialSmoothingAlgorithm)
		{
			result = SingleExponentialSmoothing(historyData);//һ��ָ��ƽ��
		}	
		else if (pa == DoubleExponentialSmoothingAlgorithm)
		{
			result = DoubleExponentialSmoothing(historyData, 1);//����ָ��ƽ��
		}
		else
		{
			result = DoubleExponentialSmoothing(historyData, 1);//����ָ��ƽ��
		}
		predictResult.insert(pair<int, int>(it->first, result));//�洢���
	}
	m_iPredictSum = 0;//Ԥ��������������
	for (map<int, int>::iterator itor = predictResult.begin(); itor != predictResult.end(); itor++)
	{
		m_iPredictSum += itor->second;
	}
}
int Predict::SingleExponentialSmoothing(vector<int> historyData)
{
	//ǰ����ֵ��ƽ��ֵ��Ϊ��ֵ
	double lastPredictValue = (historyData[0] + historyData[1] + historyData[2]) / 3.0;
	double predictValue;
	for (int i = 1; i <= historyData.size(); i++)
	{
		predictValue = m_iAlphaS1 * historyData[i - 1] + (1 - m_iAlphaS1)*lastPredictValue;
		lastPredictValue = predictValue;
	}
	return (int)(predictValue + 0.5);
}

int Predict::DoubleExponentialSmoothing(vector<int> historyData, int steps)
{
	//ǰ����ֵ��ƽ��ֵ��Ϊ��ֵ
	double lastPredictValueS1 = (historyData[0] + historyData[1] + historyData[2]) / 3.0;
	double lastPredictValueS2 = lastPredictValueS1;
	double predictValueS1, predictValueS2;
	for (int i = 1; i <= historyData.size(); i++)
	{
		predictValueS1 = m_iAlphaS1 * historyData[i - 1] + (1 - m_iAlphaS1)*lastPredictValueS1;
		predictValueS2 = m_iAlphaS2 * predictValueS1 + (1 - m_iAlphaS2) * lastPredictValueS2;
		lastPredictValueS1 = predictValueS1;
		lastPredictValueS2 = predictValueS2;
	}
	double At = 2 * predictValueS1 - lastPredictValueS2;
	double Bt = m_iAlphaS2 / (1 - m_iAlphaS2)*(predictValueS1 - predictValueS2);
	double result = At + Bt*steps;
	if (result < 0)
		result = 0;
	return (int)(result + 0.5);
}

int Predict::CubicExponentialSmoothing(vector<int> historyData, int steps)
{
	//ǰ����ֵ��ƽ��ֵ��Ϊ��ֵ
	double lastPredictValueS1 = (historyData[0] + historyData[1] + historyData[2]) / 3.0;
	double lastPredictValueS2 = lastPredictValueS1;
	double lastPredictValueS3 = lastPredictValueS1;
	double predictValueS1, predictValueS2, predictValueS3;
	for (int i = 1; i <= historyData.size(); i++)
	{
		predictValueS1 = m_iAlphaS1 * historyData[i - 1] + (1 - m_iAlphaS1)*lastPredictValueS1;
		predictValueS2 = m_iAlphaS2 * predictValueS1 + (1 - m_iAlphaS2) * lastPredictValueS2;
		predictValueS3 = m_iAlphaS3 * predictValueS2 + (1 - m_iAlphaS2) * lastPredictValueS3;
		lastPredictValueS1 = predictValueS1;
		lastPredictValueS2 = predictValueS2;
		lastPredictValueS3 = predictValueS3;
	}
	double At = 3 * predictValueS1 - 3 * predictValueS2 + predictValueS3;
	double Bt = m_iAlphaS3 / 2 / (1 - m_iAlphaS3) / (1-m_iAlphaS3)* \
		((6 - 5 * m_iAlphaS3)*predictValueS1 - (10 - 8 * m_iAlphaS3)*predictValueS2 + (4 - 3 * m_iAlphaS3)*lastPredictValueS3);
	double Ct = m_iAlphaS3*m_iAlphaS3 / 2 / (1 - m_iAlphaS3) / (1 - m_iAlphaS3)* \
		(predictValueS1 - 2 * predictValueS2 + predictValueS3);
	double result = At + Bt*steps + Ct*steps*steps;
	if (result < 0)
		result = 0;
	return (int)(result + 0.5);
}
//��ӡ���
string Predict::PrintResult(class DataProcessing &dp)
{
	//Ԥ��������������
	m_sPredictResult += to_string(m_iPredictSum);
	m_sPredictResult += "\n";
	//Ԥ����
	for (map<int, int>::iterator it = predictResult.begin(); it != predictResult.end(); it++)
	{
		m_sPredictResult += "flavor";
		m_sPredictResult += to_string(dp.flavorMap[it->first].m_iType);
		m_sPredictResult += " ";
		m_sPredictResult += to_string(it->second);
		m_sPredictResult += "\n";
	}
	cout << m_sPredictResult;
	return m_sPredictResult;
}