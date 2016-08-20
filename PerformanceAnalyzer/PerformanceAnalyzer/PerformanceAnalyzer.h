#include <iostream>
using namespace std;

#include<string.h>
#include<time.h>
#include<map>
#include <stdarg.h>
#include <assert.h>
#include <windows.h>
// C++11
#include <mutex>

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

// 获取当前线程id
static int GetThreadId()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return thread_self();
#endif
}

enum option
{
	PPCO_NONE = 0,                  // 不做剖析
	PPCO_PROFILER = 2,              // 开启剖析
	PPCO_SAVE_TO_CONSOLE = 4,       // 保存到控制台
	PPCO_SAVE_TO_FILE = 8,          // 保存到文件
	PPCO_SAVE_BY_CALL_COUNT = 16,   // 按调用次数降序保存
	PPCO_SAVE_BY_COST_TIME = 32     // 按调用花费时间降序保存
};

//// 配置管理
//class ConfigManager :public Singleton<ConfigManager>
//{
//public:
//	void SetOptions(int options)
//	{
//		_options = options;
//	}
//
//	int GetOptions()
//	{
//		return _options;
//	}
//
//private:
//	ConfigManager()
//		:_options(PPCO_NONE)
//	{}
//
//	int _options;
//};

///////////////////////////////////
//           性能剖析            //
///////////////////////////////////
struct PPNode
{
	string _filename; // 文件名
	string _function; // 函数名
	int _line;        // 行号
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
		: _TotalCallCount(0)
		, _TotalCostTime(0)
		, _TotalRefCount(0)
	{}

	void Begin(int threadId)
	{
		// 加锁
		unique_lock<mutex> Lock(_mutex);

		if (++_refCountMap[threadId] == 1)
		{
			_beginTimeMap[threadId] = clock();
		}

		_callCountMap[threadId]++;
		++_TotalCostTime;
		++_TotalCallCount;
		++_TotalRefCount;
	}

	void End(int threadId)
	{
		unique_lock<mutex> Lock(_mutex);

		LongType refCount = --_refCountMap[threadId];

		// 引用计数 == 0时更新剖析段花费时间
		// 解决剖析递归程序的问题和剖析段不匹配的问题
		if (refCount == 0)
		{
			map<int, LongType>::iterator it = _beginTimeMap.find(threadId);
			if (it != _beginTimeMap.end())
			{
				LongType costTime = clock() - it->second;

				if (refCount == 0)
					_costTimeMap[threadId] += costTime;
				else
					_costTimeMap[threadId] = costTime;

				_TotalCostTime += costTime;
			}
		}

		--_TotalRefCount;
	}

	//加锁
	mutex _mutex;					     // 互斥锁

	map<int, LongType> _beginTimeMap;    // 开始时间统计
	map<int, LongType> _costTimeMap;     // 花费时间统计
	map<int, LongType> _callCountMap;    // 调用次数统计
	map<int, LongType> _refCountMap;     // 引用计数

	int _TotalCostTime;          //总花费时间
	int _TotalCallCount;         //总调用次数
	int _TotalRefCount;          //总引用计数
};

template <class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		// 双重检查保障线程安全和效率
		if (_sInstance == NULL)
		{
			unique_lock<mutex> lock(_mutex);
			if (_sInstance == NULL)
			{
				_sInstance = new T();
			}
		}
		return _sInstance;
	}

protected:
	Singleton()
	{}
	static T* _sInstance;     // 单实例对象
	static mutex _mutex;      // 互斥锁对象
};

template <class T>
T* Singleton<T>::_sInstance = NULL;

template <class T>
mutex Singleton<T>::_mutex;

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
		map<PPNode, PPSection*>::iterator ppIt = _ppMap.begin();
		while (ppIt != _ppMap.end())
		{
			sa.Save("NO%d, Desc:%s\n", num++, ppIt->first._desc.c_str());
			sa.Save("Filename:%s, Function:%s, Line:%d\n", 
				ppIt->first._filename.c_str(),
				ppIt->first._function.c_str(),
				ppIt->first._line);

			const PPNode& Node = ppIt->first;
			PPSection* Section = ppIt->second;
			map<int, LongType>::iterator efIt = Section->_costTimeMap.begin();
			while (efIt != Section->_costTimeMap.end())
			{
				int id = efIt->first;
				sa.Save("ThreadId:%d, CostTime:%.2f, CallCount:%d\n"
					, id
					,(double)efIt->second / 1000
					,Section->_callCountMap[id]);

				++efIt;
			}
			sa.Save("TotalCostTime:%.2f, TotalCallCount:%d\n\n", (double)Section->_TotalCostTime/1000, Section->_TotalCallCount);

			++ppIt;
		}
	}

protected:
	map<PPNode, PPSection*> _ppMap;
	mutex _mutex;
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
	sign##section->Begin(GetThreadId());

#define PERFORMANCE_PROFILER_EE_END(sign) \
	sign##section->End(GetThreadId());