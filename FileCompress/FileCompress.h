#pragma once

//#define _CRT_SECURE_NO_WARNINGS 1

#include <string>
#include <assert.h>
#include <algorithm>
#include <iostream>

using namespace std;
#pragma warning(disable:4996)

typedef long LongType;

//文件信息结构体做堆成员
struct FileInfo
{
	unsigned char _ch;   //字符
	LongType _count;     //字符出现的次数，权值
	string _code;        //Huffman编码

	FileInfo(unsigned char ch = 0)
		:_ch(ch)
		, _count(0)
		, _code("")
	{}

	//以_count做权值，所以需重载 < 
	bool operator < (const FileInfo& info)
	{
		return this->_count < info._count;
	}

	bool operator != (const int& illegal)const
	{
		if (this->_count != illegal)
			return true;
		return false;
	}

	FileInfo operator + (const FileInfo& info)
	{
		FileInfo FI;
		FI._count = this->_count + info._count;
		return FI;
	}
};

class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
		{
			_infors[i]._ch = (unsigned char)i;
		}
	}
public:
	bool Compress(const char* filename)
	{
		//1.打开文件，统计字符出现的次数
		//2.生成对应的HUffman编码
		//3.压缩文件
		//4.写配置文件，方便后续的解压缩

		assert(filename);
		FILE* fOut = fopen(filename, "rb");
		assert(fOut);

		//统计字符出现的次数
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_infors[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}

		//生成对应的HuffmanCode
		HuffmanTree<FileInfo> HT;
		HuffmanTreeNode<FileInfo>* root = HT.CreateTree(_infors, 256, 0);
		_GenerateHuffmanCode(root);

		//3.压缩文件
		CompressFile(filename);

		//4.写配置文件，方便后续的解压缩
		ConfigFile(filename);
		
		return true;
	}

	//解压缩
	bool  NoCompress(const char* filename)
	{
		//读配置文件，依照次数建树
		assert(filename);

		//读配置文件
		string ConfigFile = filename;
		ConfigFile = "config." + ConfigFile;
		FILE* fOutConfig = fopen(ConfigFile.c_str(), "rb");
		assert(fOutConfig);

		//读压缩文件
		string CompressFile = filename;
		CompressFile = "Compress." + CompressFile;
		FILE* fOutCompress = fopen(CompressFile.c_str(), "rb");
		assert(fOutCompress);

		//读压缩文件
		string NoCompressFile = filename;
		NoCompressFile = "NoCompress." + NoCompressFile;
		FILE* fInNoCompress = fopen(NoCompressFile.c_str(), "wb");
		assert(fInNoCompress);

		//读配置文件，依照次数建树
		//每次读一行
		//long long size = 0;  atoi ()    ==>  int 
		//size >> 32  
		//size |= int
		//size <<= 32

		//用文件个数作为终止条件
		long long size = 0;
		string str1 = GetLine(fOutConfig);
		int tmp = atoi(str1.c_str());
		size >>= 32;
		size |= tmp;
		size <<= 32;
		string str2 = GetLine(fOutConfig);
		tmp = atoi(str2.c_str());
		size |= tmp;

		str1.clear();
		str2.clear();

		string str = GetLine(fOutConfig);
		while (!str.empty())
		{
			string str2;
			str2 = str.substr(2);
			_infors[str[0]]._count = atoi(str2.c_str());
			str.clear();
			str = GetLine(fOutConfig);
		}

		//创建哈弗曼树
		HuffmanTree<FileInfo> HT;
		HuffmanTreeNode<FileInfo>* root = HT.CreateTree(_infors, 256, 0);
		HuffmanTreeNode<FileInfo>* cur = root;
		
		//解压缩文件
		int flag = 0;
		char ch = fgetc(fOutCompress);
		while (cur && !feof(fOutCompress))
		{
			//拿出一个字符然后进行操作
			int index = 0;
			while (cur && index < 8)
			{
				char CHAR = 1;
				CHAR = (CHAR << index) & ch;

				if (CHAR)
					cur = cur->_right;
				else
					cur = cur->_left;

				if (cur && cur->_left == NULL && cur->_right == NULL)
				{
					fputc((unsigned char)cur->_weight._ch, fInNoCompress);
					++flag;
					if (flag == size)
					{
						break;
					}
					cur = root;
				}
				++index;
			}
			
			ch = fgetc(fOutCompress);
		}

		fclose(fOutConfig);
		fclose(fOutCompress);
		fclose(fInNoCompress);
		return true;
	}
public:
	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>* root)
	{
		if (NULL == root)
			return;

		_GenerateHuffmanCode(root->_left);
		_GenerateHuffmanCode(root->_right);

		//如果当前节点是叶子节点，则生成对应的哈夫曼编码
		if (root->_left == NULL && root->_right == NULL)
		{
			HuffmanTreeNode<FileInfo>* cur = root;
			HuffmanTreeNode<FileInfo>* parent = cur->_parent;
			string& code = _infors[cur->_weight._ch]._code;

			while (parent)
			{
				if (parent->_left == cur)
				{
					code += '0';
				}
				else
				{
					code += '1';
				}
				cur = cur->_parent;
				parent = cur->_parent;
			}

			//逆置huffamanCode
			reverse(code.begin() , code.end());
			//cout << root->_weight._ch << ":" << code << " "<<endl;
		}
	}

	void CompressFile(const char* filename)
	{
		assert(filename);

		FILE* fOut = fopen(filename, "rb");
		assert(fOut);

		string CompressFile = filename;
		CompressFile = "Compress." + CompressFile;
		FILE* fInCompress = fopen(CompressFile.c_str(), "wb");
		assert(fInCompress);

		int flag = 0;  //一个code操作完后的所在位置
		unsigned char CHAR = 0;    //要存入的字符

		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			int index = 0;      //八个一组的下标
			
			string code = _infors[(unsigned char)ch]._code;
			while (index < code.size())
			{
				unsigned char tmp = code[index] - '0';
				tmp <<= flag;
				CHAR = CHAR | tmp;
				if (flag == 7)
				{
					//将ch写入文件
					fputc(CHAR, fInCompress);
					CHAR = 0;
				}
				++index;
				++flag;
				if (flag == 8)
				{
					flag -= 8;
				}
			}

			ch = fgetc(fOut);
		}

		//文件结束后加0
		if (flag != 0)
		{
			fputc(CHAR, fInCompress);
		}

		fclose(fOut);
		fclose(fInCompress);
	}


	//配置文件格式如：a,2
	void ConfigFile(const char* filename)
	{
		assert(filename);

		FILE* fOut = fopen(filename, "rb");
		assert(fOut);

		string NoCompressFile = filename;
		NoCompressFile ="config." + NoCompressFile;
		FILE* fInNoCompress = fopen(NoCompressFile.c_str(), "wb");
		assert(fInNoCompress);

		int Height = 0;       //高32位
		int Low = 0;          //低32位
		long long size = GetSize(fOut);
		Low |= (size & 0xffffffff);
		Height |= size >> 32;

		char str1[128];
		_itoa_s(Low, str1, 10);  //低位存储在第一行
		fputs(str1, fInNoCompress);
		fputc('\n', fInNoCompress);
		//

		char str2[128];
		_itoa_s(Height, str2, 10); //高位存储在第二行
		fputs(str2, fInNoCompress);
		fputc('\n', fInNoCompress);
		//delete[] str2;

		//利用infors
		for (int i = 0; i < 256; ++i)
		{
			if (_infors[i]._count)
			{
				char buffer[128];
				fputc(_infors[i]._ch, fInNoCompress);
				fputc(',', fInNoCompress);

				_itoa_s(_infors[i]._count, buffer, 10);
				fputs(buffer, fInNoCompress);

				//delete[] buffer;
				fputc('\n', fInNoCompress);
			}
		}

		fclose(fOut);
		fclose(fInNoCompress);
		//delete[] str1;
		//delete[] str2;
		//delete[] buffer;
	}

	//解压缩时按行读
	string GetLine(FILE* fOut)
	{
		assert(fOut);

		string str = "";

		char ch = fgetc(fOut);
		while (ch != EOF && ch != '\n')
		{
			str += ch;
			ch = fgetc(fOut);
		}
		return str;
	}

	//统计源文件字符的总个数，原因：压缩文件是按8 位存储，最后八位可能没存储满，（我这里补零）
	//如果不统计源文件次数，最后的0也会解压缩（一般解压为堆顶元素），结果就会多出几个字符
	long long GetSize(FILE* fOut)
	{
		assert(fOut);
		long long size = 0;
		for (int i = 0; i < 256; ++i)
		{
			if (_infors[i]._count)
			{
				size = size + _infors[i]._count;
			}
		}
		return size;
	}

private:
	FileInfo _infors[256];
};

void TestHuffmanCompress1()
{
	FileCompress FC;
    
	int begin1 = GetTickCount();
	FC.Compress("123.txt");
	int end1 = GetTickCount();
	int begin2 = GetTickCount();
	FC.NoCompress("123.txt");
	int end2 = GetTickCount();

	cout << "Compress:" << end1 - begin1 << endl;
	cout << "NoCompare:" << end2 - begin2 << endl;
}