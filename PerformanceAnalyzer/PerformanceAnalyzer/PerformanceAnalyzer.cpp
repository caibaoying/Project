#include "PerformanceAnalyzer.h"

PPSection* PerformanceAnalyzer::CreateSection(const char* filename, const char* function, \
	int line, const char* desc)
{
	unique_lock<mutex> Lock(_mutex);
	PPNode ppnode(filename, function, line, desc);
	PPSection* ppsection = NULL;

	map<PPNode, PPSection*>::iterator it = _ppMap.find(ppnode);

	if (it != _ppMap.end())
	{
		ppsection = it->second;
	}
	else
	{
		ppsection = (PPSection*)new PPSection();
		_ppMap.insert(pair<PPNode, PPSection*> (ppnode, ppsection));
	}

	return ppsection;
}

bool PerformanceAnalyzer::CompareByCallCount(map<PPNode, PPSection*>::iterator l, map<PPNode, PPSection*>::iterator r)
{
	return l->second->_TotalCallCount > r->second->_TotalCallCount;
}

bool PerformanceAnalyzer::CompareByCostTime(map<PPNode, PPSection*>::iterator l, map<PPNode, PPSection*>::iterator r)
{
	return l->second->_TotalCostTime > r->second->_TotalCostTime;
}

void PerformanceAnalyzer::_Output(SaveAdapter& sa)
{
	sa.Save("===========Performance Profiler Report===========\n\n");
	//sa.Save("Profiler Begin Time :%s\n", ctime(&_ppMap.begin()->second->_beginTimeMap[i]));

	unique_lock<mutex> Lock(_mutex);

	vector<map<PPNode, PPSection*>::iterator> vinfos;
	int num = 1;
	map<PPNode, PPSection*>::iterator ppIt = _ppMap.begin();

	if (ppIt == _ppMap.end())
	{
		printf("MAP 里没有东西\n");
		return;
	}
	while (ppIt != _ppMap.end())
	{
		vinfos.push_back(ppIt);
		++ppIt;
	}

	if (vinfos.size() == 0)
	{
		printf("vinfos error\n");
		return;
	}

	//按照配置条件对剖析结果进行排序输出
	int flag = ConfigManager::GetInstance()->GetOptions();
	if (flag & PPCO_SAVE_BY_COST_TIME)
	{
		sort(vinfos.begin(), vinfos.end(), CompareByCostTime);
	}
	else if (flag & PPCO_SAVE_BY_CALL_COUNT)
	{
		sort(vinfos.begin(), vinfos.end(), CompareByCallCount);
	}
	else
	{
	}

	int index = 0;
	for (index = 0; index < vinfos.size(); ++index)
	{
		sa.Save("NO%d, Desc:%s\n", num++, vinfos[index]->first._desc.c_str());
		sa.Save("Filename:%s, Function:%s, Line:%d\n",
			vinfos[index]->first._filename.c_str(),
			vinfos[index]->first._function.c_str(),
			vinfos[index]->first._line);

		const PPNode& Node = vinfos[index]->first;
		PPSection* Section = vinfos[index]->second;
		map<int, LongType>::iterator efIt = Section->_costTimeMap.begin();
		while (efIt != Section->_costTimeMap.end())
		{
			int id = efIt->first;
			sa.Save("ThreadId:%d, CostTime:%.2f, CallCount:%d\n"
				, id
				, (double)efIt->second / 1000
				, Section->_callCountMap[id]);

			++efIt;
		}
		sa.Save("TotalCostTime:%.2f, TotalCallCount:%d\n\n", (double)Section->_TotalCostTime / 1000, Section->_TotalCallCount);
	}
}