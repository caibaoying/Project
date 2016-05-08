#pragma once
#pragma warning(disable:4018)

#include <assert.h>

template <class T>
//哈夫曼树的节点
struct HuffmanTreeNode
{
	HuffmanTreeNode<T>* _left;       //左孩子
	HuffmanTreeNode<T>* _right;      //右孩子
	HuffmanTreeNode<T>* _parent;     //指向父节点，方便解压回溯遍历
	T _weight;                       //权值

	HuffmanTreeNode(const T& weight)
		:_weight(weight)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

//仿函数
//权值weight比较,传给Heap让小堆的形成要按照权值的大小进行比较得出
template <class T>
struct NodeCompress
{
	bool operator()(HuffmanTreeNode<T>* a, const HuffmanTreeNode<T>* b)
	{
		return a->_weight < b->_weight;
	}
};
//构建哈弗曼树
template <class T>
//用class 编不过
struct HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;

	HuffmanTree()
	    :_root(NULL)
	{}

	~HuffmanTree()
	{}

	//先序遍历
	void PrevHuffmanTree()
	{
		_PrevHuffmanTree(_root);
	}

public:
	//建造HuffmanTree
	//建大堆,由于以次数建树，所以权值大的，靠近堆顶，HuffmanCode短，
	Node* CreateTree(const T* a, size_t size, int illegal)
	{
		assert(a);

		MinHeap<Node*, NodeCompress<T>> minHeap;
		for (int i = 0; i < size; ++i)
		{
			if (a[i] != illegal)
			{
				Node* node = new Node(a[i]);
				minHeap.Push(node);
			}
		}

		//不能用empty判空
		while (minHeap.Size() > 1)
		{
			//拿出权值最小的两个数
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node*  right= minHeap.Top();
			minHeap.Pop();
			 
			//权值之和建根节点
			Node* parent = new Node(left->_weight + right->_weight);

			//链接
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
			
			_root = parent;

			//将根节点push进堆
			minHeap.Push(parent);
		}
		return _root;
	}
protected:
	void _PrevHuffmanTree(Node* root)
	{
		Node* cur = root;
		if (cur == NULL)
		{
			return;
		}
		cout << cur->_weight << " ";
		_PrevHuffmanTree(cur->_left);
		_PrevHuffmanTree(cur->_right);
	}
protected:
	Node* _root;
};

//测试哈夫曼树的正确性
void TestHuffmanTree()
{
	int array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	HuffmanTree<int> hf;
	hf.CreateTree(array, sizeof(array) / sizeof(int), -1);
	hf.PrevHuffmanTree();
	cout << endl;
}
