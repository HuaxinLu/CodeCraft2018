#include "predict.h"
#include <stdio.h>
#include "DataProcessing.h"
#include "MyPredict.h"
#include "Packing.h"

//��Ҫ��ɵĹ��������
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	string myResult;
	//����Ԥ����������ȡ�ͻ��ֵ�Ԫ
	DataProcessing dp(info, MAX_INFO_NUM, data, data_num);
	dp.ReadInputData();
	dp.ReadTrainData();
	dp.SplitTrainData();
	//Ԥ�⣬����ʹ�ö���ָ��ƽ��Ԥ��
	Predict pr;
	pr.PredictStart(dp, DoubleExponentialSmoothingAlgorithm);
	myResult += pr.PrintResult(dp);
	//װ�䣬̰��+װ��
	Packing pk(dp,pr);
	pk.PackingStart();
	myResult += pk.PrintResult();

	//cout << myResult;

	// ��Ҫ���������
	const char * result_file = myResult.c_str();

	// ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
	write_result(result_file, filename);
}
