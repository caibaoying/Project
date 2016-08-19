#include <iostream>
using namespace std;

#include<string.h>
#include<time.h>
#include<map>
#include <stdarg.h>
#include <assert.h>

typedef long long LongType;
///////////////////////////////////////////////////////////////////
// 保存适配器
class SaveAdapter
{
public:
	virtual void Save(const char* format, ...) = 0;
};

class ConsoleSaveAdapter :public SaveAdapter
{
public:
	virtual void Save(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf(stdout, format, args);
		va_end(args);
	}
};

class FileSaveAdapter :public SaveAdapter
{
public:
	FileSaveAdapter(const char* filename)
	{
		fopen_s(&_fout, filename, "w");
		assert(_fout);
	}

	~FileSaveAdapter()
	{
		if (_fout)
		{
			fclose(_fout);
		}
	}

	virtual void Save(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf(_fout, format, args);
		va_end(args);
	}

protected:
	// 防拷贝
	FileSaveAdapter(const FileSaveAdapter&);
	FileSaveAdapter& operator=(const FileSaveAdapter&);

protected:
	FILE* _fout;
};

///////////////////////////////////
//           性能剖析            //
///////////////////////////////////
struct PPNode
{
	string _filename; // 文件名
	string _function; // 函数名
	int _line;         // 行号

	string _desc;     // 描述

	PPNode(const string& filename, const string& function, int line, const string& desc)
		:_filename(filename)
		, _function(function)
		, _line(line)
		, _desc(desc)
	{}

	bool operator < (const PPNode& ppnode)const
	{
		if (_filename < ppnode._filename)
		{
			return true;
		}
		else if (_filename > ppnode._filename)
		{
			return false;
		}
		else
		{
			if (_function < ppnode._function)
			{
				return true;
			}
			else if (_function > ppnode._function)
			{
				return false;
			}
			else
			{
				if (_line < ppnode._line)
				{
					return true;
				}
				else if (_line > ppnode._line)
				{
					return false;
				}
			}
		}
		return false;
	}

	bool operator == (const PPNode& ppnode)const
	{
		return ((_filename == ppnode._filename) && \
			(_function == ppnode._function) && \
			(_line == ppnode._line));
	}
};

struct PPSection
{
public:
	PPSection()
		:_begintime(0)
		, _costtime(0)
		, _callcount(0)
		, _refCount(0)
	{}

	void Begin()
	{
		if (_refCount == 0)
		{
			_begintime = clock();
		}

		++_refCount;
		++_callcount;
	}

	void End()
	{
		--_refCount;
		if (_refCount == 0)
		{
			_costtime += clock() - _begintime;
		}
	}

	//加锁
	map<int, LongType> _beginTimeMap;
	map<int, LongType> _costTimeMap;
	map<int, LongType> _callCountMap;
	map<int, LongType> _refCountMap;

	int _begintime;
	int _costtime;
	int _callcount;
	int _refCount;          //引用计数
};

template <class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		assert(_sInstance);
		return _sInstance;
	}

protected:
	static T* _sInstance;
};

template <class T>
T* Singleton<T>::_sInstance = new T;

class PerformanceAnalyzer:public Singleton<PerformanceAnalyzer>
{
	friend class Singleton<PerformanceAnalyzer>;
public:
	PPSection* CreateSection(const char* filename, const char* function, int line, const char* desc);

	void Output()
	{
		ConsoleSaveAdapter csa;
		_Output(csa);

		FileSaveAdapter fsa("PerformanceProfilerReport.txt");
		_Output(fsa);
	}

protected:
	void _Output(SaveAdapter& sa)
	{
		int num = 1;
		map<PPNode, PPSection*>::iterator it = _ppMap.begin();
		while (it != _ppMap.end())
		{
			sa.Save("NO%d, Desc:%s\n", num++, it->first._desc.c_str());
			sa.Save("Filename:%s, Function:%s, Line:%d\n", 
				it->first._filename.c_str(),
				it->first._function.c_str(),
				it->first._line);

			sa.Save("CostTime:%.2f, CallCount:%d\n", (double)it->second->_costtime/1000, 
				it->second->_callcount);
			
			++it;
		}
	}

protected:
	map<PPNode, PPSection*> _ppMap;
};

struct Release
{
	~Release()
	{
		PerformanceAnalyzer::GetInstance()->Output();
	}
};

#define PERFORMANCE_PROFILER_EE_BEGIN(sign, desc) \
	PPSection* sign##section = PerformanceAnalyzer::GetInstance()->CreateSection(__FILE__, __FUNCTION__, __LINE__, desc); \
	sign##section->Begin();

#define PERFORMANCE_PROFILER_EE_END(sign) \
	sign##section->End();