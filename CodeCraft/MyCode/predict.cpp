#include "predict.h"
#include <stdio.h>
#include "DataProcessing.h"
#include "MyPredict.h"
#include "Packing.h"

//你要完成的功能总入口
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	string myResult;
	//数据预处理，包括读取和划分单元
	DataProcessing dp(info, MAX_INFO_NUM, data, data_num);
	dp.ReadInputData();
	dp.ReadTrainData();
	dp.SplitTrainData();
	//预测，这里使用二次指数平滑预测
	Predict pr;
	pr.PredictStart(dp, DoubleExponentialSmoothingAlgorithm);
	myResult += pr.PrintResult(dp);
	//装箱，贪婪+装箱
	Packing pk(dp,pr);
	pk.PackingStart();
	myResult += pk.PrintResult();

	//cout << myResult;

	// 需要输出的内容
	const char * result_file = myResult.c_str();

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(result_file, filename);
}
