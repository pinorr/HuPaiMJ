#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>

#include "HuPaiMJ2.h"
#include "HuPaiMJ3.h"

using namespace std;

void print_cards(bool bSuc1, bool bSuc2, BYTE* cards)
{
	printf("%d%d   ", bSuc1, bSuc2);
	for (int i = 0; i<9; ++i)
	{
		printf("%d,", cards[i]);
	}
	printf("  ");

	for (int i = 9; i<18; ++i)
	{
		printf("%d,", cards[i]);
	}
	printf("  ");
	for (int i = 18; i<27; ++i)
	{
		printf("%d,", cards[i]);
	}
	printf("  ");
	for (int i = 27; i<34; ++i)
	{
		printf("%d,", cards[i]);
	}
	printf("\n");
}

#define GUI_NUM			3
#define MAX_COUNT (100 * 10000)
static BYTE g_HuCardAll[136];

CHuPaiMJ stTssss;
ArrayMJ::CHuPaiArrayMJ stArray;

void test_repeat()
{
	BYTE cards[] = {
		0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 4,
	};
	int hu = stArray.CheckCanHu(cards, 33);
	cout << "hu:" << hu << endl;
}
BYTE source[MAX_COUNT * 9 * 34];

void main()
{
	stArray.TrainAll();

	test_repeat();

	stTssss.TrainAll();
	
	for (int i = 0; i < 34; i++)
	{
		g_HuCardAll[i * 4] = i;
		g_HuCardAll[i * 4+1] = i;
		g_HuCardAll[i * 4+2] = i;
		g_HuCardAll[i * 4+3] = i;
	}
	
	int gui_index = 33;
	int total = 0;
	srand(11);
	for (int n = 0; n < MAX_COUNT; ++n)
	{
		random_shuffle(g_HuCardAll, g_HuCardAll + 130);		// 这个函数对计算有影响
		for (int i = 0; i < 9; ++i)	// 136/14 -> 9
		{
			BYTE* cards = &source[total++ * 34];
			memset(cards, 0, 34);
			for (int j = i * 14; j < i * 14 + 14 - GUI_NUM; j++)
				++cards[g_HuCardAll[j]];
			cards[gui_index] = GUI_NUM;			
		}
	}

	printf("gui_index=%d  GUI_NUM=%d\n", gui_index, GUI_NUM);

// 	// 判断是否结果一致
// 	for (int n = 0; n < total; ++n)
// 	{		
// 		bool bSuc1 = stTssss.CheckCanHu(source + n * 34, gui_index);
// 		bool bSuc2 = stArray.CheckCanHu(source + n * 34, gui_index);
// 		if (bSuc1 != bSuc2)		
// 			print_cards(bSuc1, bSuc2, source + n * 34);
// 	}

	// rjc查表法		
	int hu = 0;
	DWORD dwTimeBegin = GetTickCount();
	for (int n = 0; n < total; ++n)
		hu += stTssss.CheckCanHu(source + n * 34, gui_index);

	cout << "rjc查表法总数:" << total / 10000 << "万次, time:" << GetTickCount() - dwTimeBegin << "ms" << endl;
	cout << "Hu: " << hu << endl;

	//	memcpy(sourceTemp, source, sizeof(sourceTemp));
	hu = 0;
	dwTimeBegin = GetTickCount();
	for (int n = 0; n < total; ++n)
		hu += stArray.CheckCanHu(source + n * 34, gui_index);

	cout << "rjc数组法总数:" << total / 10000 << "万次, time:" << GetTickCount() - dwTimeBegin << "ms" << endl;
	cout << "Hu: " << hu << endl;

	cin >> hu;
}
