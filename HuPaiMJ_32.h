/********************************************************************
	created:	pinorr
	file base:	HuPaiMJ.h
	author:		pinorr	Q 505971450
	purpose:	麻将胡牌提示算法(第2版)
*********************************************************************/

#ifndef __HU_PAI_MJ_H__
#define __HU_PAI_MJ_H__
#include <map>
#include <set>
#include <unordered_set>
#include <string>
#include <vector>
#include <list>
#include <windows.h>
#include <iostream>
#include "DefineHuTip.h"

using namespace std;

#define MAX_KEY_NUM						(MAX_VAL_NUM+1)		//9+赖子
#define MAX_NAI_NUM						4					//赖子最大个数
#define BIT_VAL_NUM						3					//一个值占的bit数
#define BIT_VAL_FLAG					0x07				//

struct stCorOut
{
	BYTE byCorType;					//
	bool byNum;						// 总张数
	BYTE byValNum[MAX_KEY_NUM];		//
};

inline void getIndexByKey2(int llVal, BYTE byIndex[MAX_KEY_NUM])
{
	for (int i=0; i<MAX_KEY_NUM; ++i)
	{
		byIndex[i] = (llVal>>(BIT_VAL_NUM*i))&BIT_VAL_FLAG;
	}
}

inline int getKey2ByIndex(BYTE byIndex[MAX_KEY_NUM])
{
	int nKey = 0;
	for (int i=0; i<MAX_KEY_NUM; ++i)
		nKey |= (int)(byIndex[i]&BIT_VAL_FLAG)<<(BIT_VAL_NUM*i);

	return nKey;
}

inline bool isValidKey(int llVal)
{
	BYTE byIndex[MAX_KEY_NUM] = {};
	getIndexByKey2(llVal, byIndex);
	if (byIndex[9] > MAX_NAI_NUM)
		return false;

	int nNum = 0;
	for (int i=0; i<MAX_KEY_NUM; ++i)
	{
		nNum += byIndex[i];
		if (byIndex[i] > 4 || nNum > 14)	//
			return false;		
	}
	return nNum > 0;
}

set<int>				g_setSingle;		//单个顺子+刻子		50个
set<int>				g_setSingleFZ;		//单个顺子+刻子		22个
set<int>				g_setSingleJiang;	//单个将			19个
set<int>				g_setSingleJiangFZ;	//单个将			15个

unordered_set<int>		g_usetHuAll;			//
unordered_set<int>		g_usetHuAllFZ;			// 风、字牌
unordered_set<int>		g_usetHuAllJiang;		// 带将
unordered_set<int>		g_usetHuAllJiangFZ;		// 带将 风、字牌


class CHuPaiMJ
{
public:
	CHuPaiMJ(){};
	~CHuPaiMJ(){};

public:	
	static void TrainSingle()			// 1.三张单独组合
	{
		BYTE byTemp[MAX_KEY_NUM] = {};
		byTemp[9] = 3;
		g_setSingle.insert(getKey2ByIndex(byTemp));
		g_setSingleFZ.insert(getKey2ByIndex(byTemp));
		// 1.1 刻子
		for (int i=0; i<MAX_VAL_NUM; ++i)
		{
			memset(byTemp, 0, MAX_KEY_NUM);
			for (int n=0; n<3; ++n)
			{
				byTemp[i] = 3-n;
				byTemp[9] = n;
				g_setSingle.insert(getKey2ByIndex(byTemp));
				if (i < 7)	//风、字牌最多7张
					g_setSingleFZ.insert(getKey2ByIndex(byTemp));
			}
		}
		// 1.2 顺子 没赖子
		for (int i=0; i<MAX_VAL_NUM-2; ++i)
		{
			memset(byTemp, 0, MAX_KEY_NUM);	
			byTemp[i]	= 1;
			byTemp[i+1]	= 1;
			byTemp[i+2]	= 1;
			g_setSingle.insert(getKey2ByIndex(byTemp));					
		}
		// 1.2 顺子 1个赖子 (2个赖子时也就是刻子)
		for (int i=0; i<MAX_VAL_NUM-2; ++i)
		{
			memset(byTemp, 0, MAX_KEY_NUM);	
			byTemp[i]	= 1;
			byTemp[i+1]	= 1;
			byTemp[9]	= 1;
			g_setSingle.insert(getKey2ByIndex(byTemp));	
			memset(byTemp, 0, MAX_KEY_NUM);	
			byTemp[9]	= 1;
			byTemp[i+1]	= 1;
			byTemp[i+2]	= 1;			
			g_setSingle.insert(getKey2ByIndex(byTemp));
			memset(byTemp, 0, MAX_KEY_NUM);	
			byTemp[i]	= 1;
			byTemp[9]	= 1;
			byTemp[i+2]	= 1;
			g_setSingle.insert(getKey2ByIndex(byTemp));
		}
		// 1.3 将牌
		memset(byTemp, 0, MAX_KEY_NUM);	
		byTemp[9] = 2;
		g_setSingleJiang.insert(getKey2ByIndex(byTemp));
		g_setSingleJiangFZ.insert(getKey2ByIndex(byTemp));
		for (int i=0; i<MAX_VAL_NUM; ++i)
		{
			memset(byTemp, 0, MAX_KEY_NUM);
			for (int n=0; n<2; ++n)
			{
				byTemp[i] = 2-n;
				byTemp[9] = n;
				g_setSingleJiang.insert(getKey2ByIndex(byTemp));
				if (i < 7)	//风、字牌最多7张
					g_setSingleJiangFZ.insert(getKey2ByIndex(byTemp));
			}
		}
	};
	static void TrainAll()
	{
		TrainSingle();

		int nAll	= 0;
		int nAllFZ	= 0;
		int nSingle[100]	= {};
		int nSingleKe[50] = {};

		set<int>::iterator iter = g_setSingle.begin();
		for (; iter != g_setSingle.end(); ++iter)
			nSingle[nAll++] = *iter;
		iter = g_setSingleFZ.begin();
		for (; iter != g_setSingleFZ.end(); ++iter)
			nSingleKe[nAllFZ++] = *iter;

		int nAllJiang	= 0;
		int nAllJiangFZ	= 0;
		int nSingleJiang[20]	 = {};
		int nSingleJiangKe[20] = {};

		iter = g_setSingleJiang.begin();
		for (; iter != g_setSingleJiang.end(); ++iter)
			nSingleJiang[nAllJiang++] = *iter;
		iter = g_setSingleJiangFZ.begin();
		for (; iter != g_setSingleJiangFZ.end(); ++iter)
			nSingleJiangKe[nAllJiangFZ++] = *iter;

		for (int i1=0; i1<nAllFZ; ++i1)
		{
			g_usetHuAllFZ.insert(nSingleKe[i1]);
			for (int i2=i1; i2<nAllFZ; ++i2)
			{								
				if (!isValidKey(nSingleKe[i1] + nSingleKe[i2])) 
					continue;

				g_usetHuAllFZ.insert(nSingleKe[i1] + nSingleKe[i2]);
				for (int i3=i2; i3<nAllFZ; ++i3)
				{					
					if (!isValidKey(nSingleKe[i1] + nSingleKe[i2] + nSingleKe[i3])) 
						continue;
					g_usetHuAllFZ.insert(nSingleKe[i1] + nSingleKe[i2] + nSingleKe[i3]);
					for (int i4=i3; i4<nAllFZ; ++i4)
					{						
						if (!isValidKey(nSingleKe[i1] + nSingleKe[i2] + nSingleKe[i3] + nSingleKe[i4])) 
							continue;
						g_usetHuAllFZ.insert(nSingleKe[i1] + nSingleKe[i2] + nSingleKe[i3] + nSingleKe[i4]);
					}
				}
			}
		}	

		for (int i=0; i<nAllJiangFZ; ++i)
		{
			g_usetHuAllJiangFZ.insert(nSingleJiangKe[i]);
			unordered_set<int>::iterator iter_u = g_usetHuAllFZ.begin();
			for (; iter_u!=g_usetHuAllFZ.end(); ++iter_u)
			{
				if (isValidKey(nSingleJiangKe[i] + *iter_u)) 
				{
					g_usetHuAllJiangFZ.insert(nSingleJiangKe[i] + *iter_u);
				}
			}
		}
		cout<<(int)sizeof(*(g_setSingleFZ.begin()))<<endl;
		cout<<"pino:"<<dec<<g_setSingleFZ.size()<<endl;
		cout<<"pino:"<<dec<<g_usetHuAllFZ.size()<<endl;
		cout<<"pino:"<<dec<<g_usetHuAllJiangFZ.size()<<endl;

		for (int i1=0; i1<nAll; ++i1)
		{
			g_usetHuAll.insert(nSingle[i1]);
			for (int i2=i1; i2<nAll; ++i2)
			{								
				if (!isValidKey(nSingle[i1] + nSingle[i2])) 
					continue;
				g_usetHuAll.insert(nSingle[i1] + nSingle[i2]);
				for (int i3=i2; i3<nAll; ++i3)
				{					
					if (!isValidKey(nSingle[i1] + nSingle[i2] + nSingle[i3])) 
						continue;
					g_usetHuAll.insert(nSingle[i1] + nSingle[i2] + nSingle[i3]);
					for (int i4=i3; i4<nAll; ++i4)
					{						
						if (!isValidKey(nSingle[i1] + nSingle[i2] + nSingle[i3] + nSingle[i4])) 
							continue;
						g_usetHuAll.insert(nSingle[i1] + nSingle[i2] + nSingle[i3] + nSingle[i4]);
					}
				}
			}
		}

		for (int i=0; i<nAllJiang; ++i)
		{
			g_usetHuAllJiang.insert(nSingleJiang[i]);
			unordered_set<int>::iterator iter_u = g_usetHuAll.begin();
			for (; iter_u!=g_usetHuAll.end(); ++iter_u)
			{
				if (isValidKey(nSingleJiang[i] + *iter_u)) 
				{
					g_usetHuAllJiang.insert(nSingleJiang[i] + *iter_u);
				}
			}
		}
		cout<<endl;
		cout<<"pino:"<<dec<<g_setSingle.size()<<endl;
		cout<<"pino:"<<dec<<g_usetHuAll.size()<<endl;
		cout<<"pino:"<<dec<<g_usetHuAllJiang.size()<<endl;

		cout<<g_setSingle.size() + g_setSingleFZ.size() + g_usetHuAll.size() + g_usetHuAllFZ.size() + g_usetHuAllJiang.size() + g_usetHuAllJiangFZ.size()<<endl;
	}	

	static bool CheckCanHuSingle(stColorData &stColor, BYTE byNaiNum)
	{
		stColor.byCount[9] = byNaiNum;
		BYTE byLeftNum = (stColor.byNum+byNaiNum)%3;
		if (byLeftNum == 1) 
			return false;

		if (stColor.byNum == 0)
			return true;
		
		if (byLeftNum == 0)
		{
			if (stColor.byCorType == enColorMJ_FenZi)
				return g_usetHuAllFZ.find(getKey2ByIndex(stColor.byCount)) != g_usetHuAllFZ.end();
			else
				return g_usetHuAll.find(getKey2ByIndex(stColor.byCount)) != g_usetHuAll.end();
		}
		else
		{
			if (stColor.byCorType == enColorMJ_FenZi)
				return g_usetHuAllJiangFZ.find(getKey2ByIndex(stColor.byCount)) != g_usetHuAllJiangFZ.end();
			else
				return g_usetHuAllJiang.find(getKey2ByIndex(stColor.byCount)) != g_usetHuAllJiang.end();				
		}
		return false;
	}	
	static bool CheckCanHu(stCardData &stData, BYTE byNaiIndex, int nNaiZiUse[enColorMJ_Max])
	{
		if (stData.byNum % 3 != 2)
			return false;

		int nNaiZiNum = 0;
		if (byNaiIndex != INVALID_VAL)
		{
			nNaiZiNum = stData.byCardNum[byNaiIndex];
			stData.byCardNum[byNaiIndex] = 0;
			stData.byNum -= nNaiZiNum;
		}

		int nNaiZiMax[enColorMJ_Max] = {4, 4, 4, 4};
		stColorData stColorTemp[enColorMJ_Max];
		for (int cor=0; cor<enColorMJ_Max; ++cor)
		{		
			stColorTemp[cor].byCorType = cor;
			int nAll = (cor == enColorMJ_FenZi) ? 7 : MAX_VAL_NUM;
			if (cor*MAX_VAL_NUM + nAll <= MAX_TOTAL_TYPE)
			{				
				memcpy(stColorTemp[cor].byCount, stData.byCardNum+cor*MAX_VAL_NUM, nAll);
				for (int i=0; i<nAll; ++i)
					stColorTemp[cor].byNum += stColorTemp[cor].byCount[i];			
			}
			if (stColorTemp[cor].byNum == 0)
				nNaiZiMax[cor] = 0;
		}

		BYTE byLeftNum[enColorMJ_Max] = {};
		for (int i1=0; i1<=min(nNaiZiMax[0], nNaiZiNum); ++i1)
		{		
			byLeftNum[enColorMJ_WAN] = (stColorTemp[0].byNum+i1)%3;
			if (byLeftNum[enColorMJ_WAN] == 1) continue;
			for (int i2=0; i2<=min(nNaiZiMax[1], nNaiZiNum-i1); ++i2)
			{
				byLeftNum[enColorMJ_TONG] = (stColorTemp[1].byNum+i2)%3;
				if (byLeftNum[enColorMJ_TONG] == 1) continue;
				for (int i3=0; i3<=min(nNaiZiMax[2], nNaiZiNum-i1-i2); ++i3)
				{
					byLeftNum[enColorMJ_TIAO] = (stColorTemp[2].byNum+i3)%3;
					if (byLeftNum[enColorMJ_TIAO] == 1) continue;
					int i4 = nNaiZiNum-i1-i2-i3;
					if (i4 <= nNaiZiMax[3])
					{
						byLeftNum[enColorMJ_FenZi] = (stColorTemp[3].byNum+i4)%3;
						if (byLeftNum[enColorMJ_FenZi] == 1 || byLeftNum[0]+byLeftNum[1]+byLeftNum[2]+byLeftNum[3] != 2) continue;

						stColorData stTemp[enColorMJ_Max];
						memcpy(stTemp, stColorTemp, sizeof(stTemp));					

						if (stColorTemp[0].byNum>0 && !CheckCanHuSingle(stTemp[0], i1))
							continue;
						if (stColorTemp[1].byNum>0 && !CheckCanHuSingle(stTemp[1], i2))
							continue;
						if (stColorTemp[2].byNum>0 && !CheckCanHuSingle(stTemp[2], i3))
							continue;
						if (stColorTemp[3].byNum>0 && !CheckCanHuSingle(stTemp[3], i4))
							continue;

						nNaiZiUse[0] = i1;
						nNaiZiUse[1] = i2;
						nNaiZiUse[2] = i3;
						nNaiZiUse[3] = i4;
						return true;
					}
				}			
			}
		}	
		return false;
	};	
};

#endif  //__CPLAYER_H__