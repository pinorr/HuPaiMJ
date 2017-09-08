#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>
#include "HuPaiMJ2.h"

using namespace std;

#define GUI_NUM		4
#define MAX_COUNT (100 * 10000)
static BYTE g_HuCardAll[136];

CHuPaiMJ stTssss;

void test_repeat()
{
	BYTE cards[] = {
		0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
	};
	int hu = 0;
	for (int i = 0; i < 10; i++){
		hu += stTssss.CheckCanHu(cards, 33);
	}
	cout << "hu:" << hu << endl;
}
BYTE source[MAX_COUNT * 9 * 34];

void main()
{
	stTssss.TrainAll();

	//test_repeat();

	for (int i = 0; i < 34; i++)
	{
		g_HuCardAll[i * 4] = i;
		g_HuCardAll[i * 4 + 1] = i;
		g_HuCardAll[i * 4 + 2] = i;
		g_HuCardAll[i * 4 + 3] = i;
	}

	int gui_index = 33;
	int total = 0;
	srand(1);
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

	// pinorr 查表法
	int hu = 0;
	DWORD dwTimeBegin = GetTickCount();
	for (int n = 0; n < total; ++n)
	{
		hu += stTssss.CheckCanHu(source + n * 34, gui_index);
	}
	cout << "pinorr 查表法总数:" << 9 * MAX_COUNT / 10000 << "万次, time:" << GetTickCount() - dwTimeBegin << "ms" << endl;
	cout << "Hu: " << hu << endl;

	cin >> hu;
}
