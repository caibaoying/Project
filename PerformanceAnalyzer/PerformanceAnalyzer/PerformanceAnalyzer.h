#include <iostream>
using namespace std;

#include<string.h>
#include<time.h>
#include<map>
#include <stdarg.h>
#include <assert.h>
#include <windows.h>
#include <vector>
#include <algorithm>
// C++11
#include <mutex>

typedef long long LongType;
///////////////////////////////////////////////////////////////////
// ����������
////////////////////////////////////////////////////////////////
class SaveAdapter
{
public:
	virtual void Save(const char* format, ...) = 0;
};

//////////////////////////////////////////////////
//    ����̨����������
///////////////////////////////////////////////////

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

//////////////////////////////////////////////////
//        �ļ�����������
///////////////////////////////////////////////////

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
	// ������
	FileSaveAdapter(const FileSaveAdapter&);
	FileSaveAdapter& operator=(const FileSaveAdapter&);

protected:
	FILE* _fout;
};

// ��ȡ��ǰ�߳�id
static int GetThreadId()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return thread_self();
#endif
}

///////////////////////////////////
//           ��������            //
///////////////////////////////////

///////////////////////////////////////////////////
//    �ļ���Ϣ�� key
struct PPNode
{
	string _filename; // �ļ���
	string _function; // ������
	int _line;        // �к�
	string _desc;     // ����

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

///////////////////////////////////////////////////
//Ч����Ϣ�� value

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
		// ����
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

		// ���ü��� == 0ʱ���������λ���ʱ��
		// ��������ݹ���������������β�ƥ�������
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

	//����
	mutex _mutex;					     // ������

	map<int, LongType> _beginTimeMap;    // ��ʼʱ��ͳ��
	map<int, LongType> _costTimeMap;     // ����ʱ��ͳ��
	map<int, LongType> _callCountMap;    // ���ô���ͳ��
	map<int, LongType> _refCountMap;     // ���ü���

	int _TotalCostTime;          //�ܻ���ʱ��
	int _TotalCallCount;         //�ܵ��ô���
	int _TotalRefCount;          //�����ü���
};

///////////////////////
//����ģʽ

template <class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		// ˫�ؼ�鱣���̰߳�ȫ��Ч��
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
	static T* _sInstance;     // ��ʵ������
	static mutex _mutex;      // ����������
};

template <class T>
T* Singleton<T>::_sInstance = NULL;

template <class T>
mutex Singleton<T>::_mutex;

//////////////////////////////////////////////////
//    ���ù���ѡ��
enum option
{
	PPCO_NONE = 0,                  // ��������
	PPCO_PROFILER = 1,              // ��������
	PPCO_SAVE_TO_CONSOLE = 2,       // ���浽����̨
	PPCO_SAVE_TO_FILE = 4,          // ���浽�ļ�
	PPCO_SAVE_BY_CALL_COUNT = 8,   // �����ô������򱣴�
	PPCO_SAVE_BY_COST_TIME = 16,     // �����û���ʱ�併�򱣴�
};

//////////////////////////////////////////////////
// ���ù���
///////////////////////////////////////////////////

class ConfigManager :public Singleton<ConfigManager>
{
	friend class Singleton<ConfigManager>;
public:
	void SetOptions(int options)
	{
		_options |= options;
	}

	int GetOptions()
	{
		return _options;
	}

private:
	ConfigManager()
		:_options(PPCO_NONE)
	{}

	int _options;
};

/////////////////////////////////////////////////
// ��Դͳ��

// ��Դͳ����Ϣ
//struct ResourceInfo
//{
//	LongType _peak;  // ����ֵ
//	LongType _avg;   // ƽ��
//
//	LongType _total; // ��ֵ
//	LongType _count; // ����
//
//	ResourceInfo()
//		:_peak(0)
//		, _avg(0)
//		, _total(0)
//		, _count(0)
//	{}
//
//	void Update(LongType value);
//	void Serialize(SaveAdapter& sa)const;
//};
//
//// ��Դͳ��
//class ResourceStatistics
//{
//public:
//};

class PerformanceAnalyzer:public Singleton<PerformanceAnalyzer>
{
	friend class Singleton<PerformanceAnalyzer>;
public:
	PPSection* CreateSection(const char* filename, const char* function, int line, const char* desc);

	void Output()
	{
		int flag = ConfigManager::GetInstance()->GetOptions();
		if (flag & PPCO_SAVE_TO_CONSOLE)
		{
			ConsoleSaveAdapter csa;
			_Output(csa);
		}
		if (flag & PPCO_SAVE_TO_FILE)
		{
			FileSaveAdapter fsa("PerformanceProfilerReport.txt");
			_Output(fsa);
		}
	}

protected:
	static bool CompareByCallCount(map<PPNode, PPSection*>::iterator l, map<PPNode, PPSection*>::iterator r);
	static bool CompareByCostTime(map<PPNode, PPSection*>::iterator l, map<PPNode, PPSection*>::iterator r);
	void _Output(SaveAdapter& sa);

protected:
	map<PPNode, PPSection*> _ppMap;
	mutex _mutex;
};

// ��ӡ
struct Release
{
	~Release()
	{
		PerformanceAnalyzer::GetInstance()->Output();
	}
};

///////////////////////////���ֺ�//////////////////////////

// ��ʼʱ��
#define PERFORMANCE_PROFILER_EE_BEGIN(sign, desc) \
	PPSection* sign##section = NULL;                         \
if (ConfigManager::GetInstance()->GetOptions() & PPCO_PROFILER)\
{                                                        \
	sign##section = PerformanceAnalyzer::GetInstance()->CreateSection(__FILE__, __FUNCTION__, __LINE__, desc); \
	sign##section->Begin(GetThreadId());                     \
}


// ����ʱ��
#define PERFORMANCE_PROFILER_EE_END(sign) \
if (sign##section)                    \
	sign##section->End(GetThreadId());

// ѡ������
#define SET_OPTION(option)\
	ConfigManager::GetInstance()->SetOptions(option);