#pragma once
#pragma warning(disable:4018)

#include <assert.h>

template <class T>
//���������Ľڵ�
struct HuffmanTreeNode
{
	HuffmanTreeNode<T>* _left;       //����
	HuffmanTreeNode<T>* _right;      //�Һ���
	HuffmanTreeNode<T>* _parent;     //ָ�򸸽ڵ㣬�����ѹ���ݱ���
	T _weight;                       //Ȩֵ

	HuffmanTreeNode(const T& weight)
		:_weight(weight)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

//�º���
//Ȩֵweight�Ƚ�,����Heap��С�ѵ��γ�Ҫ����Ȩֵ�Ĵ�С���бȽϵó�
template <class T>
struct NodeCompress
{
	bool operator()(HuffmanTreeNode<T>* a, const HuffmanTreeNode<T>* b)
	{
		return a->_weight < b->_weight;
	}
};
//������������
template <class T>
//��class �಻��
struct HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;

	HuffmanTree()
	    :_root(NULL)
	{}

	~HuffmanTree()
	{}

	//�������
	void PrevHuffmanTree()
	{
		_PrevHuffmanTree(_root);
	}

public:
	//����HuffmanTree
	//�����,�����Դ�������������Ȩֵ��ģ������Ѷ���HuffmanCode�̣�
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

		//������empty�п�
		while (minHeap.Size() > 1)
		{
			//�ó�Ȩֵ��С��������
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node*  right= minHeap.Top();
			minHeap.Pop();
			 
			//Ȩֵ֮�ͽ����ڵ�
			Node* parent = new Node(left->_weight + right->_weight);

			//����
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
			
			_root = parent;

			//�����ڵ�push����
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

//���Թ�����������ȷ��
void TestHuffmanTree()
{
	int array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	HuffmanTree<int> hf;
	hf.CreateTree(array, sizeof(array) / sizeof(int), -1);
	hf.PrevHuffmanTree();
	cout << endl;
}
