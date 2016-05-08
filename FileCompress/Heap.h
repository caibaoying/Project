#pragma once

#pragma warning(disable:4018)

#include <vector>
#include<iostream>
using namespace std;


//仿函数
template <class T>
struct Less
{
	//重载 < ,如果是string类型，自定义string，string需重载 < 
	bool operator()(const T& a, const T& b)const
	{
		return a < b;
	}
};

template <class T>
struct Greater
{
	//重载 >
	bool operator()(const T& a, const T& b)const
	{
		return a > b;
	}
};

template <class T,class compare = NodeCompress<T>>
class MinHeap
{
public:
	MinHeap()
		:_array(NULL)
	{}
	// 最小堆
	//向上调整
	MinHeap(const vector<T>& array)
	{
		for (size_t i = 0; i < array.size(); ++i)
		{
			_array.push_back(array[i]);
		}
		moveUp(_array);
	}

	//打印结果
	void Display()
	{
		for (size_t i = 0; i < _array.size(); ++i)
		{
			cout << _array[i] << " ";
		}
		cout << endl;
	}

	//push(x)后向上调整
	void Push(const T& x)
	{
		_array.push_back(x);
		moveUp(_array);
	}

	void Pop()
	{
		if (!_array.empty())
		{
			swap(_array[0], _array[_array.size() - 1]);
			_array.pop_back();
			moveDown(0);
		}
	}

	//接口，得到堆顶的值，即最小值
	T& Top()
	{
		if (_array[0])
		{
			return _array[0];
		}
	}

	size_t Size()
	{
		return _array.size();
	}

	~MinHeap()
	{}
public:
	bool IsEmpty()const
	{
		if (_array.empty())
		{
			return true;
		}
	}

	//向上调整
	void moveUp(vector<T>& array)
	{
		int child = array.size() - 1;
		int parent = (child-1) / 2;
		while (child > 0)
		{
			//如果右孩子存在，且小于左孩子
			if (child + 1 < array.size() && (compare()(array[child+1] , array[child])))
			{
				++child;
			}

			//较小的一个和根节点比较
			if (compare()(array[child] , array[parent]))
			{
				swap(array[parent], array[child]);
				moveDown(child);
			}
			--parent;
			child = parent*2 + 1;
		}
	}

	//向上调整后，必须要向下调整，树的高度大于二可能出现bug,所以此举是确保万无一失
	//原理与向上相似，方向相反
	void moveDown(int parent)
	{
		int child = (parent) * 2 + 1;
		while (child < _array.size())
		{
			if (child + 1 < _array.size() && compare()(_array[child + 1] , _array[child]))
			{
				++child;
			}
			if (compare()(_array[child] , _array[parent]))
			{
				swap(_array[parent], _array[child]);
				parent = child;
				child = child * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
private:
	vector<T> _array;
};

//测试堆
void testheap()
{
	vector<int> arr = { 10, 16, 18, 12, 9, 13, 15, 17, 14, 11 };
	MinHeap<int,Less<int>> heap(arr);
	heap.Display();

	heap.Push(2);
	heap.Display();

	heap.Pop();
	heap.Display();
}

