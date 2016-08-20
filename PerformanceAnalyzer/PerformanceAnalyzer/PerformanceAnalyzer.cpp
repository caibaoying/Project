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