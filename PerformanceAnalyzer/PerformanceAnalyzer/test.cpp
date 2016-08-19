#include "PerformanceAnalyzer.h"
#include <windows.h>
#include <thread>

void Run(int n)
{
	while (n--)
	{
		PERFORMANCE_PROFILER_EE_BEGIN(network, "���紫��");
		Sleep(500);
		PERFORMANCE_PROFILER_EE_END(network);

		PERFORMANCE_PROFILER_EE_BEGIN(mid, "�м��߼�");
		Sleep(500);
		PERFORMANCE_PROFILER_EE_END(mid);

		PERFORMANCE_PROFILER_EE_BEGIN(sql, "���ݿ�");		
		Sleep(500);
		PERFORMANCE_PROFILER_EE_END(sql);
	}
}

void TestMutilThread()
{
	Release tmp;
	thread t1(Run, 1);
	thread t2(Run, 2);
	thread t3(Run, 3);

	t1.join();
	t2.join();
	t3.join();
}

//PerformanceAnalyzer pp;
//
//void Test()
//{
//	PPSection* s1 = pp.CreateSection(__FILE__, __FUNCTION__, __LINE__, "���ݿ�");
//	s1->Begin();
//	Sleep(500);
//	s1->End();
//
//	PPSection* s2 = pp.CreateSection(__FILE__, __FUNCTION__, __LINE__, "����");
//	s2->Begin();
//	Sleep(1000);
//	s2->End();
//}
int main()
{
	//Test();
	TestMutilThread();
	system("pause");
	return 0;
}