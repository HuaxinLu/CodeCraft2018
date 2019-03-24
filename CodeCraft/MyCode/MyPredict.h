#ifndef MY_PREDICT_H_
#define MY_PREDICT_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

//ʹ���㷨ѡ��
enum PredictAlgorithm
{
	SingleExponentialSmoothingAlgorithm,
	DoubleExponentialSmoothingAlgorithm,
	CubicExponentialSmoothingAlgorithm
};

class DataProcessing;
class Packing;

class Predict
{
public:
	friend class Packing;

	Predict();
	void PredictStart(class DataProcessing &dp, enum PredictAlgorithm pa);
	string PrintResult(class DataProcessing &dp);

private:
	int SingleExponentialSmoothing(vector<int> historyData);
	int DoubleExponentialSmoothing(vector<int> historyData, int steps);
	int CubicExponentialSmoothing(vector<int> historyData, int steps);

	double m_iAlphaS1;//һ��Ȩ��
	double m_iAlphaS2;//����Ȩ��
	double m_iAlphaS3;//����Ȩ��
	map<int, int> predictResult;//Ԥ���� flavor����������ӳ��
	int m_iPredictSum;//Ԥ��������������
	string m_sPredictResult;//����ַ���
};
#endif