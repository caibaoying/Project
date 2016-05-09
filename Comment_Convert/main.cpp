#include <stdio.h>

extern int Comment_Convert(FILE *inputfile,FILE *outputfile);

int main()
{
	FILE *fpIn;
    FILE *fpOut;
	fpIn = fopen("input.cpp","r");
    fpOut = fopen("output.cpp","w");
	if(NULL == fpIn)
	{
		printf("Error: Open input.cpp file fail");
		return -1;
	}
    if(NULL == fpIn)
	{
		printf("Error: Open output.cpp file fail");
		return -1;
	}
	Comment_Convert(fpIn,fpOut);
	fclose(fpIn);
	fclose(fpOut);
	return 0;
}