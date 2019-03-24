#include "Packing.h"
#include "MyPredict.h"
#include "DataProcessing.h"
Packing::Packing(class DataProcessing &dp, class Predict &pr)
{
	map<int, int>::iterator itor = pr.predictResult.begin();
	//�����ر�������ת��Ϊ01��������
	for (; itor != pr.predictResult.end(); itor++)
	{
		FlavorPredict fp;
		fp.m_iType = itor->first;
		fp.m_iCpuCount = dp.flavorMap[itor->first].m_iCpuCount;
		fp.m_iMemCount = dp.flavorMap[itor->first].m_iMemCount;
		for (int i = 0; i < itor->second;i++)
			flavorPredictVec.push_back(fp);
	}
	//��ʼ��һЩҪ�õ��Ĳ���
	m_iServerCpuCount = dp.problemInfo.m_iServerCpuCount;
	m_iServerMemCount = dp.problemInfo.m_iServerMemCount;
	m_iFlavorNumber = pr.m_iPredictSum;
	m_iRestFlavor = pr.m_iPredictSum;
	m_iFlavorTypes = dp.problemInfo.m_iFlavorCount;
}

void Packing::PackingStart()
{
	//ѭ����ֹ����������Ϊ��
	while (!flavorPredictVec.empty())
	{
		int cpuRest = 0;
		int memRest = 0;
		ServerUsed su;//�½�һ������
		//����ʣ�µ��������Mem��Cpu
		vector<FlavorPredict>::iterator itor = flavorPredictVec.begin();
		for (; itor != flavorPredictVec.end(); itor++)
		{
			cpuRest += itor->m_iCpuCount;
			memRest += itor->m_iMemCount;
		}
		if (cpuRest <= m_iServerCpuCount && memRest <= m_iServerMemCount)//ֻ��Ҫһ������������װ��
		{
			vector<FlavorPredict>::iterator itorFinal = flavorPredictVec.begin();
			for (; itorFinal != flavorPredictVec.end(); itorFinal++)
			{
				PackOneFlavor(su, itorFinal->m_iType);//ʣ�µ�ȫ��װ�����������ȥ
			}
			serverUsedVec.push_back(su);
			return;
		}
		//����һ��װ�꣬Ҫ���ж�̬�滮
		m_iRestFlavor = flavorPredictVec.size();//����ʣ������������
		//�����滮����
		int*** dp = new int**[m_iRestFlavor];
		for (int i = 0; i < m_iRestFlavor; i++)//��һά ���������
		{
			dp[i] = new int*[m_iServerCpuCount + 1];
			for (int j = 0; j < m_iServerCpuCount + 1; j++)//�ڶ�ά CPU����
			{
				dp[i][j] = new int[m_iServerMemCount + 1];//����ά MEM����
			}
		}

		//��ʼ����һ��,ֱ���ж��ܲ��ܷ���
		for (int i = 0; i < m_iServerCpuCount + 1; i++)
		{
			for (int j = 0; j < m_iServerMemCount + 1; j++)
			{
				dp[0][i][j] = ((flavorPredictVec[0].m_iCpuCount <= i) && (flavorPredictVec[0].m_iMemCount <= j)) ? \
					flavorPredictVec[0].m_iCpuCount : 0;
			}
		}

		//��̬�滮
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
		//����
		//����ÿһ��flavor���ж��Ƿ�ʹ��
		int flavorNumber = m_iRestFlavor - 1;
		int lastCpuCount = m_iServerCpuCount;
		int lastMemCount = m_iServerMemCount;
		for (; flavorNumber > 0; flavorNumber--)
		{
			if (dp[flavorNumber][lastCpuCount][lastMemCount] != dp[flavorNumber - 1][lastCpuCount][lastMemCount])
			{
				//˵������������ʹ��
				int usedFlavorType = flavorPredictVec[flavorNumber].m_iType;//��¼һ����̨�������type
				//��ӵ�װ�䷽����
				PackOneFlavor(su, usedFlavorType);
				//����ǰһ����״̬
				lastCpuCount -= flavorPredictVec[flavorNumber].m_iCpuCount;
				lastMemCount -= flavorPredictVec[flavorNumber].m_iMemCount;
				//ɾ����������
				flavorPredictVec.erase(flavorPredictVec.begin() + flavorNumber);
			}
		}
		//��һ��Ҫ�����ж�
		if (dp[flavorNumber][lastCpuCount][lastMemCount] != 0)
		{
			int usedFlavorType = flavorPredictVec[flavorNumber].m_iType;
			PackOneFlavor(su, usedFlavorType);
			//ɾ����������
			flavorPredictVec.erase(flavorPredictVec.begin() + flavorNumber);
		}
		serverUsedVec.push_back(su);
		//�ͷ�
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

//��һ�������װ��
void Packing::PackOneFlavor(ServerUsed &su, int flavorType)
{
	if (su.flavorInside.count(flavorType) == 0)
		su.flavorInside.insert(pair<int, int>(flavorType, 1));
	else
		su.flavorInside[flavorType]++;
}