#pragma once

//#define _CRT_SECURE_NO_WARNINGS 1

#include <string>
#include <assert.h>
#include <algorithm>
#include <iostream>

using namespace std;
#pragma warning(disable:4996)

typedef long LongType;

//�ļ���Ϣ�ṹ�����ѳ�Ա
struct FileInfo
{
	unsigned char _ch;   //�ַ�
	LongType _count;     //�ַ����ֵĴ�����Ȩֵ
	string _code;        //Huffman����

	FileInfo(unsigned char ch = 0)
		:_ch(ch)
		, _count(0)
		, _code("")
	{}

	//��_count��Ȩֵ������������ < 
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
		//1.���ļ���ͳ���ַ����ֵĴ���
		//2.���ɶ�Ӧ��HUffman����
		//3.ѹ���ļ�
		//4.д�����ļ�����������Ľ�ѹ��

		assert(filename);
		FILE* fOut = fopen(filename, "rb");
		assert(fOut);

		//ͳ���ַ����ֵĴ���
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_infors[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}

		//���ɶ�Ӧ��HuffmanCode
		HuffmanTree<FileInfo> HT;
		HuffmanTreeNode<FileInfo>* root = HT.CreateTree(_infors, 256, 0);
		_GenerateHuffmanCode(root);

		//3.ѹ���ļ�
		CompressFile(filename);

		//4.д�����ļ�����������Ľ�ѹ��
		ConfigFile(filename);
		
		return true;
	}

	//��ѹ��
	bool  NoCompress(const char* filename)
	{
		//�������ļ������մ�������
		assert(filename);

		//�������ļ�
		string ConfigFile = filename;
		ConfigFile = "config." + ConfigFile;
		FILE* fOutConfig = fopen(ConfigFile.c_str(), "rb");
		assert(fOutConfig);

		//��ѹ���ļ�
		string CompressFile = filename;
		CompressFile = "Compress." + CompressFile;
		FILE* fOutCompress = fopen(CompressFile.c_str(), "rb");
		assert(fOutCompress);

		//��ѹ���ļ�
		string NoCompressFile = filename;
		NoCompressFile = "NoCompress." + NoCompressFile;
		FILE* fInNoCompress = fopen(NoCompressFile.c_str(), "wb");
		assert(fInNoCompress);

		//�������ļ������մ�������
		//ÿ�ζ�һ��
		//long long size = 0;  atoi ()    ==>  int 
		//size >> 32  
		//size |= int
		//size <<= 32

		//���ļ�������Ϊ��ֹ����
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

		//������������
		HuffmanTree<FileInfo> HT;
		HuffmanTreeNode<FileInfo>* root = HT.CreateTree(_infors, 256, 0);
		HuffmanTreeNode<FileInfo>* cur = root;
		
		//��ѹ���ļ�
		int flag = 0;
		char ch = fgetc(fOutCompress);
		while (cur && !feof(fOutCompress))
		{
			//�ó�һ���ַ�Ȼ����в���
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

		//�����ǰ�ڵ���Ҷ�ӽڵ㣬�����ɶ�Ӧ�Ĺ���������
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

			//����huffamanCode
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

		int flag = 0;  //һ��code������������λ��
		unsigned char CHAR = 0;    //Ҫ������ַ�

		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			int index = 0;      //�˸�һ����±�
			
			string code = _infors[(unsigned char)ch]._code;
			while (index < code.size())
			{
				unsigned char tmp = code[index] - '0';
				tmp <<= flag;
				CHAR = CHAR | tmp;
				if (flag == 7)
				{
					//��chд���ļ�
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

		//�ļ��������0
		if (flag != 0)
		{
			fputc(CHAR, fInCompress);
		}

		fclose(fOut);
		fclose(fInCompress);
	}


	//�����ļ���ʽ�磺a,2
	void ConfigFile(const char* filename)
	{
		assert(filename);

		FILE* fOut = fopen(filename, "rb");
		assert(fOut);

		string NoCompressFile = filename;
		NoCompressFile ="config." + NoCompressFile;
		FILE* fInNoCompress = fopen(NoCompressFile.c_str(), "wb");
		assert(fInNoCompress);

		int Height = 0;       //��32λ
		int Low = 0;          //��32λ
		long long size = GetSize(fOut);
		Low |= (size & 0xffffffff);
		Height |= size >> 32;

		char str1[128];
		_itoa_s(Low, str1, 10);  //��λ�洢�ڵ�һ��
		fputs(str1, fInNoCompress);
		fputc('\n', fInNoCompress);
		//

		char str2[128];
		_itoa_s(Height, str2, 10); //��λ�洢�ڵڶ���
		fputs(str2, fInNoCompress);
		fputc('\n', fInNoCompress);
		//delete[] str2;

		//����infors
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

	//��ѹ��ʱ���ж�
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

	//ͳ��Դ�ļ��ַ����ܸ�����ԭ��ѹ���ļ��ǰ�8 λ�洢������λ����û�洢�����������ﲹ�㣩
	//�����ͳ��Դ�ļ�����������0Ҳ���ѹ����һ���ѹΪ�Ѷ�Ԫ�أ�������ͻ��������ַ�
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