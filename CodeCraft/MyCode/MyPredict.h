#ifndef MY_PREDICT_H_
#define MY_PREDICT_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

//使用算法选择
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

	double m_iAlphaS1;//一次权重
	double m_iAlphaS2;//二次权重
	double m_iAlphaS3;//三次权重
	map<int, int> predictResult;//预测结果 flavor——个数的映射
	int m_iPredictSum;//预测出的虚拟机总数
	string m_sPredictResult;//结果字符串
};
#endif