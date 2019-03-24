#include "Packing.h"
#include "MyPredict.h"
#include "DataProcessing.h"
Packing::Packing(class DataProcessing &dp, class Predict &pr)
{
	map<int, int>::iterator itor = pr.predictResult.begin();
	//将多重背包问题转化为01背包问题
	for (; itor != pr.predictResult.end(); itor++)
	{
		FlavorPredict fp;
		fp.m_iType = itor->first;
		fp.m_iCpuCount = dp.flavorMap[itor->first].m_iCpuCount;
		fp.m_iMemCount = dp.flavorMap[itor->first].m_iMemCount;
		for (int i = 0; i < itor->second;i++)
			flavorPredictVec.push_back(fp);
	}
	//初始化一些要用到的参数
	m_iServerCpuCount = dp.problemInfo.m_iServerCpuCount;
	m_iServerMemCount = dp.problemInfo.m_iServerMemCount;
	m_iFlavorNumber = pr.m_iPredictSum;
	m_iRestFlavor = pr.m_iPredictSum;
	m_iFlavorTypes = dp.problemInfo.m_iFlavorCount;
}

void Packing::PackingStart()
{
	//循环终止条件：数组为空
	while (!flavorPredictVec.empty())
	{
		int cpuRest = 0;
		int memRest = 0;
		ServerUsed su;//新建一个箱子
		//计算剩下的虚拟机的Mem和Cpu
		vector<FlavorPredict>::iterator itor = flavorPredictVec.begin();
		for (; itor != flavorPredictVec.end(); itor++)
		{
			cpuRest += itor->m_iCpuCount;
			memRest += itor->m_iMemCount;
		}
		if (cpuRest <= m_iServerCpuCount && memRest <= m_iServerMemCount)//只需要一个服务器就能装完
		{
			vector<FlavorPredict>::iterator itorFinal = flavorPredictVec.begin();
			for (; itorFinal != flavorPredictVec.end(); itorFinal++)
			{
				PackOneFlavor(su, itorFinal->m_iType);//剩下的全部装到这个箱子里去
			}
			serverUsedVec.push_back(su);
			return;
		}
		//不能一次装完，要进行动态规划
		m_iRestFlavor = flavorPredictVec.size();//重置剩余的虚拟机数量
		//建立规划数组
		int*** dp = new int**[m_iRestFlavor];
		for (int i = 0; i < m_iRestFlavor; i++)//第一维 虚拟机数量
		{
			dp[i] = new int*[m_iServerCpuCount + 1];
			for (int j = 0; j < m_iServerCpuCount + 1; j++)//第二维 CPU数量
			{
				dp[i][j] = new int[m_iServerMemCount + 1];//第三维 MEM数量
			}
		}

		//初始化第一行,直接判断能不能放下
		for (int i = 0; i < m_iServerCpuCount + 1; i++)
		{
			for (int j = 0; j < m_iServerMemCount + 1; j++)
			{
				dp[0][i][j] = ((flavorPredictVec[0].m_iCpuCount <= i) && (flavorPredictVec[0].m_iMemCount <= j)) ? \
					flavorPredictVec[0].m_iCpuCount : 0;
			}
		}

		//动态规划
		for (int i = 1; i < m_iRestFlavor; i++)
		{
			for (int j = 0; j < m_iServerCpuCount + 1; j++)
			{
				for (int k = 0; k < m_iServerMemCount + 1; k++)
				{
					dp[i][j][k] = dp[i - 1][j][k];
					if (flavorPredictVec[i].m_iCpuCount <= j && flavorPredictVec[i].m_iMemCount <= k)
					{
						int d = dp[i - 1][j - flavorPredictVec[i].m_iCpuCount][k - flavorPredictVec[i].m_iMemCount] + flavorPredictVec[i].m_iCpuCount;
						dp[i][j][k] = dp[i][j][k] > d ? dp[i][j][k] : d;
					}
				}
			}
		}
		//回溯
		//遍历每一个flavor，判断是否使用
		int flavorNumber = m_iRestFlavor - 1;
		int lastCpuCount = m_iServerCpuCount;
		int lastMemCount = m_iServerMemCount;
		for (; flavorNumber > 0; flavorNumber--)
		{
			if (dp[flavorNumber][lastCpuCount][lastMemCount] != dp[flavorNumber - 1][lastCpuCount][lastMemCount])
			{
				//说明这个虚拟机被使用
				int usedFlavorType = flavorPredictVec[flavorNumber].m_iType;//记录一下这台虚拟机的type
				//添加到装箱方案中
				PackOneFlavor(su, usedFlavorType);
				//回溯前一步的状态
				lastCpuCount -= flavorPredictVec[flavorNumber].m_iCpuCount;
				lastMemCount -= flavorPredictVec[flavorNumber].m_iMemCount;
				//删除这个虚拟机
				flavorPredictVec.erase(flavorPredictVec.begin() + flavorNumber);
			}
		}
		//第一个要单独判断
		if (dp[flavorNumber][lastCpuCount][lastMemCount] != 0)
		{
			int usedFlavorType = flavorPredictVec[flavorNumber].m_iType;
			PackOneFlavor(su, usedFlavorType);
			//删除这个虚拟机
			flavorPredictVec.erase(flavorPredictVec.begin() + flavorNumber);
		}
		serverUsedVec.push_back(su);
		//释放
		for (int i = 0; i < m_iRestFlavor; i++)
		{
			for (int j = 0; j < m_iServerCpuCount + 1; j++)
			{
				delete[]dp[i][j];
			}
			delete[]dp[i];
		}
		delete[]dp;
	}
}

string Packing::PrintResult()
{
	m_sPackingResult += "\n";
	m_sPackingResult += to_string(serverUsedVec.size());
	m_sPackingResult += "\n";
	int i = 1;
	for (vector<ServerUsed>::iterator it1 = serverUsedVec.begin(); it1 != serverUsedVec.end(); it1++)
	{
		m_sPackingResult += to_string(i);
		m_sPackingResult += " ";
		for (map<int, int>::iterator it2 = it1->flavorInside.begin(); it2 != it1->flavorInside.end(); it2++)
		{
			m_sPackingResult += "flavor";
			m_sPackingResult += to_string(it2->first);
			m_sPackingResult += " ";
			m_sPackingResult += to_string(it2->second);
			m_sPackingResult += " ";
		}
		i++;
		m_sPackingResult += "\n";
	}
	cout << m_sPackingResult;
	return m_sPackingResult;
}

//将一个虚拟机装箱
void Packing::PackOneFlavor(ServerUsed &su, int flavorType)
{
	if (su.flavorInside.count(flavorType) == 0)
		su.flavorInside.insert(pair<int, int>(flavorType, 1));
	else
		su.flavorInside[flavorType]++;
}