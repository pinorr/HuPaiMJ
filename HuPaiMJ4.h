/********************************************************************
created:	pinorr
file base:	HuPaiMJ.h
author:		pinorr	Q 505971450
purpose:	麻将胡牌算法(第5版)
*********************************************************************/

#ifndef __HU_PAI_MJ4_H__
#define __HU_PAI_MJ4_H__
#include <set>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <unordered_map>  

using namespace std;

namespace FinalMJ
{	
	#define MAX_TOTAL_TYPE					34
	#define MAX_VAL_NUM						9
	#define MAX_KEY_NUM						(MAX_VAL_NUM+1)			//9+赖子
	#define MAX_NAI_NUM						4						//赖子最大个数
	#define BIT_VAL_NUM						3						//一个值占的bit数
	#define BIT_VAL_FLAG					0x07					//

	//麻将颜色（种类）定义
	enum enColorMJ
	{
		enColorMJ_WAN = 0,  //万
		enColorMJ_TONG,     //筒
		enColorMJ_TIAO,     //条
		enColorMJ_FenZi,    //风、字 牌
		enColorMJ_Max,
	};

	set<int>							g_setSingle;		//单个顺子+刻子		50个
	set<int>							g_setSingleFZ;		//单个顺子+刻子		22个
	set<int>							g_setSingleJiang;	//单个将			19个
	set<int>							g_setSingleJiangFZ;	//单个将			15个

	BYTE								g_byArray[262144];
	BYTE								g_byArrayFZ[262144];
	unordered_map<int, BYTE>			g_mapHu4All;
	unordered_map<int, BYTE>			g_mapHu4AllFZ;

	unordered_map<int, BYTE>			g_mapHuAll[15];
	unordered_map<int, BYTE>			g_mapHuAllFZ[15];

	inline int getKeyByIndex(BYTE byIndex[MAX_KEY_NUM], BYTE byNum = MAX_KEY_NUM)
	{
		int nKey = 0;
		for (int i = 0; i < byNum; ++i)
			nKey |= (int)(byIndex[i] & BIT_VAL_FLAG) << (BIT_VAL_NUM*i);
		return nKey;
	}
	inline int getArrayIndex(BYTE byIndex[MAX_VAL_NUM], BYTE byNum = MAX_VAL_NUM)
	{
		int nKey = 0;
		for (int i = 0; i < byNum; ++i)
		{
			if ((byIndex[i] & BIT_VAL_FLAG) > 3) byIndex[i] -= 3;
			nKey |= (int)(byIndex[i] & 0x03) << (2 * i);
		}
		return nKey;
	}
	inline int getArrayIndex(int llVal, BYTE &byMax)
	{
		byMax = 0;
		BYTE byIndex[MAX_VAL_NUM] = {};
		for (int i = 0; i < MAX_VAL_NUM; ++i) {
			byIndex[i] = (llVal >> (BIT_VAL_NUM*i))&BIT_VAL_FLAG;
			byMax = max(byMax, byIndex[i]);
		}
		return getArrayIndex(byIndex);
	}
	inline bool isValidKey(int llVal)
	{
		BYTE byIndex[MAX_KEY_NUM] = {};
		for (int i = 0; i < MAX_KEY_NUM; ++i)
			byIndex[i] = (llVal >> (BIT_VAL_NUM*i))&BIT_VAL_FLAG;

		if (byIndex[9] > MAX_NAI_NUM)	return false;
		int nNum = 0;
		for (int i = 0; i < MAX_KEY_NUM; ++i)
		{
			nNum += byIndex[i];
			if (byIndex[i] > 4 || nNum > 14)	//
				return false;
		}
		return nNum > 0;
	}
	inline BYTE getNumByKey(int llVal, BYTE byNum = MAX_KEY_NUM)
	{
		BYTE byIndex[MAX_KEY_NUM] = {};
		for (int i = 0; i < MAX_KEY_NUM; ++i)
			byIndex[i] = (llVal >> (BIT_VAL_NUM*i))&BIT_VAL_FLAG;

		BYTE nNum = 0;
		for (int i = 0; i < byNum; ++i)
			nNum += byIndex[i];
		return nNum;
	}
	inline void addMap(unordered_map<int, BYTE> mapTemp[], int llVal)
	{
		BYTE nNum = getNumByKey(llVal, MAX_VAL_NUM);
		BYTE byNum = (llVal >> (BIT_VAL_NUM * 9))&BIT_VAL_FLAG;
		int  val = (llVal & 0x7FFFFFF);
		unordered_map<int, BYTE>::iterator iter = mapTemp[nNum].find(val);
		if (iter != mapTemp[nNum].end())
			iter->second = min(byNum, iter->second);
		else
			mapTemp[nNum][val] = byNum;		
	}

	class CHuPaiArrayMJ
	{
	private:
		static void TrainSingle()
		{
			BYTE byTemp[MAX_KEY_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 };
			g_setSingle.insert(getKeyByIndex(byTemp));
			g_setSingleFZ.insert(getKeyByIndex(byTemp));
			// 1.1 刻子
			for (int i = 0; i < MAX_VAL_NUM; ++i)
			{
				memset(byTemp, 0, MAX_KEY_NUM);
				for (int n = 0; n < 3; ++n)
				{
					byTemp[i] = 3 - n;	byTemp[9] = n;
					g_setSingle.insert(getKeyByIndex(byTemp));
					if (i < 7)	//风、字牌最多7张
						g_setSingleFZ.insert(getKeyByIndex(byTemp));
				}
			}
			// 1.2 顺子 没赖子
			for (int i = 0; i < MAX_VAL_NUM - 2; ++i)
			{
				memset(byTemp, 0, MAX_KEY_NUM);
				byTemp[i] = 1;	byTemp[i + 1] = 1;	byTemp[i + 2] = 1;
				g_setSingle.insert(getKeyByIndex(byTemp));
			}
			// 1.3 顺子 1个赖子 (2个赖子时也就是刻子)
			for (int i = 0; i < MAX_VAL_NUM - 2; ++i)
			{
				for (int n = 0; n < 3; ++n)
				{
					memset(byTemp, 0, MAX_KEY_NUM);
					byTemp[i] = 1;	byTemp[i + 1] = 1;	byTemp[i + 2] = 1;
					byTemp[i + n] = 0;	byTemp[9] = 1;
					g_setSingle.insert(getKeyByIndex(byTemp));
				}
			}
			// 2.1 将牌
			memset(byTemp, 0, MAX_KEY_NUM);
			byTemp[9] = 2;
			g_setSingleJiang.insert(getKeyByIndex(byTemp));
			g_setSingleJiangFZ.insert(getKeyByIndex(byTemp));
			for (int i = 0; i < MAX_VAL_NUM; ++i)
			{
				memset(byTemp, 0, MAX_KEY_NUM);
				for (int n = 0; n < 2; ++n)
				{
					byTemp[i] = 2 - n;	byTemp[9] = n;
					g_setSingleJiang.insert(getKeyByIndex(byTemp));
					if (i < 7)	//风、字牌最多7张
						g_setSingleJiangFZ.insert(getKeyByIndex(byTemp));
				}
			}
		};

		static void TrainAllComb(const set<int> &setSingle, unordered_map<int, BYTE> mapOut[])
		{
			int nAll = 0;
			int nSingle[100] = {};
			set<int>::iterator iter = setSingle.begin();
			for (; iter != setSingle.end(); ++iter)
				nSingle[nAll++] = *iter;

			for (int i1 = 0; i1 < nAll; ++i1)
			{
				addMap(mapOut, nSingle[i1]);
				for (int i2 = i1; i2 < nAll; ++i2)
				{
					int nTemp = nSingle[i1] + nSingle[i2];
					if (!isValidKey(nTemp))	continue;
					addMap(mapOut, nTemp);
					for (int i3 = i2; i3 < nAll; ++i3)
					{
						int nTemp = nSingle[i1] + nSingle[i2] + nSingle[i3];
						if (!isValidKey(nTemp))	continue;
						addMap(mapOut, nTemp);
						for (int i4 = i3; i4 < nAll; ++i4)
						{
							int nTemp = nSingle[i1] + nSingle[i2] + nSingle[i3] + nSingle[i4];
							if (!isValidKey(nTemp))	continue;
							addMap(mapOut, nTemp);
						}
					}
				}
			}
		}

		static void TrainAllComb_Jiang(const set<int> &setSingle, unordered_map<int, BYTE> mapOut[])
		{
			int nAll = 0;
			int nSingle[100] = {};

			set<int>::iterator iter = setSingle.begin();
			for (; iter != setSingle.end(); ++iter)
				nSingle[nAll++] = *iter;

			unordered_map<int, BYTE> mapTemp[15];
			for (int j = 0; j < 15; ++j)
				mapTemp[j] = mapOut[j];

			for (int i = 0; i < nAll; ++i)
			{
				for (int j = 0; j < 15; ++j)
				{
					addMap(mapOut, nSingle[i]);
					unordered_map<int, BYTE>::iterator iter_u = mapTemp[j].begin();
					for (; iter_u != mapTemp[j].end(); ++iter_u)
					{
						int nTemp = nSingle[i] + iter_u->first + (int(iter_u->second & BIT_VAL_FLAG) << 27);
						if (isValidKey(nTemp))
							addMap(mapOut, nTemp);
					}
				}
			}
		}

	public:
		static void TrainAll()
		{
			if (g_setSingle.empty())
			{
				memset(g_byArray, 0xFF, sizeof(g_byArray));
				memset(g_byArrayFZ, 0xFF, sizeof(g_byArrayFZ));
								
				DWORD dwFlag = GetTickCount();
				TrainSingle();
				TrainAllComb(g_setSingle, g_mapHuAll);
				TrainAllComb(g_setSingleFZ, g_mapHuAllFZ);
				TrainAllComb_Jiang(g_setSingleJiang, g_mapHuAll);
				TrainAllComb_Jiang(g_setSingleJiangFZ, g_mapHuAllFZ);

				BYTE byMax = 0;
				int numAll = 0;
				for (int i = 0; i < 15; ++i)
				{
					numAll += g_mapHuAll[i].size();
					numAll += g_mapHuAllFZ[i].size();
				}
				cout << "train cost:" << GetTickCount() - dwFlag << "ms numAll=" << numAll << endl;
				for (int i = 0; i < 15; ++i)
				{
					unordered_map<int, BYTE>::iterator iter = g_mapHuAll[i].begin();
					for (; iter != g_mapHuAll[i].end(); ++iter)
					{
						int nArrayIndex = getArrayIndex(iter->first, byMax);
						if (byMax < 4)
							g_byArray[nArrayIndex] = iter->second;
						else
							g_mapHu4All[iter->first] = iter->second;
					}
					iter = g_mapHuAllFZ[i].begin();
					for (; iter != g_mapHuAllFZ[i].end(); ++iter)
					{
						int nArrayIndex = getArrayIndex(iter->first, byMax);
						if (byMax < 4)
							g_byArrayFZ[nArrayIndex] = iter->second;
						else
							g_mapHu4AllFZ[iter->first] = iter->second;
					}
					g_mapHuAll[i].clear();
					g_mapHuAllFZ[i].clear();
				}
			}
			else
				cout << "already trained!" << endl;
		}

		static bool CheckCanHu(BYTE byCardSrc[], BYTE byNaiIndex)
		{
			BYTE byCards[MAX_TOTAL_TYPE];
			memcpy(byCards, byCardSrc, MAX_TOTAL_TYPE);
			int nNaiZiNum = 0;
			if (byNaiIndex < MAX_TOTAL_TYPE)
			{
				nNaiZiNum = byCards[byNaiIndex];
				byCards[byNaiIndex] = 0;
			}

			BYTE byJiangNum = 0;
			BYTE nNaiTry;
			for (int cor = 0; cor < enColorMJ_Max; ++cor)
			{				
				int nMax = (cor == enColorMJ_FenZi) ? 7 : 9;
				int nVal = 0, nNum = 0;

				bool isArrayFlag = true;
				BYTE byDelIndex = 0xFF, byTemp = 0;
				for (int i = 0; i < nMax; ++i)
				{
					byTemp = byCards[9 * cor + i];
					nNum += byTemp;
					if (byTemp > 3)
					{
						isArrayFlag = false;
						byDelIndex = i;
						nVal |= (int)(byTemp - 3) << (2 * i);
					}						
					else
						nVal |= (int)(byTemp) << (2 * i);
				}

				if (nNum == 0) continue;

				int	 nVal4 = getKeyByIndex(byCards + 9 * cor, MAX_VAL_NUM);
				if (cor == enColorMJ_FenZi)
					nVal4 &= 0x1FFFFF;

				if (cor == enColorMJ_FenZi) {
					if (isArrayFlag)
						nNaiTry = g_byArrayFZ[nVal];
					else {
						auto iter = g_mapHu4AllFZ.find(nVal4);
						if (iter != g_mapHu4AllFZ.end())
							nNaiTry = iter->second;
						else
							nNaiTry = 0xFF;
					}
				}
				else {
					if (isArrayFlag)
						nNaiTry = g_byArray[nVal];
					else {
						auto iter = g_mapHu4All.find(nVal4);
						if (iter != g_mapHu4All.end())
							nNaiTry = iter->second;
						else
							nNaiTry = 0xFF;		
					}
				}				

				if (nNaiTry != 0xFF)
					byJiangNum += ((nNum + nNaiTry) % 3 == 2);
				
				if (nNaiTry == 0xFF || nNaiZiNum < nNaiTry || byJiangNum + nNaiTry > nNaiZiNum + 1)
				{
					if (byDelIndex != 0xFF)
					{
						byCards[9 * cor + byDelIndex] -= 2;
						--cor; ++byJiangNum;
						continue;
					}
					return false;
				}
				nNaiZiNum -= nNaiTry;				
			}
			return byJiangNum > 0 || nNaiZiNum >= 2;
		}
	};
}

#endif //__HU_PAI_MJ4_H__
