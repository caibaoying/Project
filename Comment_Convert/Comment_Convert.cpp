#include <stdio.h>

//例举状态
typedef enum                
{
	NO_COMMENT_STATE,
	C_COMMENT_STATE,
	CPP_COMMENT_STATE,
    PERSIST_STATE,               //双引号
	SINGLE_MARK,                 //单引号
	END_STATE
}STATE;

//双引号
typedef enum
{
	NO_FLAG,
	C_FLAG,
	CPP_FLAG		
}FLAG;

//单引号
typedef enum
{
	S_NO_FLAG,
	S_C_FLAG,
	S_CPP_FLAG		
}SG_FLAG;

//定义状态机
typedef struct STATE_MACHINE
{
	FILE *inputfile;
	FILE *outputfile;
	STATE ulstate;
	FLAG flag;
	SG_FLAG sg_flag;
}STATE_MACHINE;

STATE_MACHINE g_state = {0};
void EventPro(char ch);
void EventProAtNO(char ch);
void EventProAtC(char ch);
void EventProAtCpp(char ch);
void EventProAtPersist(char ch);
void EventProAtSingle_Mark(char ch);

int Comment_Convert(FILE *inputfile,FILE *outputfile)
{
	if(NULL==inputfile || NULL==outputfile)
	{
		printf("Arguement illegal");
		return -1;
	}
	g_state.inputfile = inputfile;
    g_state.outputfile = outputfile;
	g_state.ulstate = NO_COMMENT_STATE;

	char ch;
	while(g_state.ulstate != END_STATE)
	{
        ch = fgetc(g_state.inputfile);
		EventPro(ch);
	}
	return 0;
}

void EventPro(char ch)
{
	switch(g_state.ulstate)
	{
	case NO_COMMENT_STATE:
        EventProAtNO(ch);
        break; 
	case C_COMMENT_STATE:
		EventProAtC(ch);
		break;
	case CPP_COMMENT_STATE:
		EventProAtCpp(ch);
		break;
	case PERSIST_STATE:
		EventProAtPersist(ch);
		break;
	case SINGLE_MARK:
		EventProAtSingle_Mark(ch);
		break;
	case END_STATE:
		break;
	default:
		break;
	}
}

void EventProAtNO(char ch)
{
	char nextch;
	switch(ch)
	{
	case '/':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '/')              //Cpp
		{
			fputc(ch,g_state.outputfile);
			fputc('*',g_state.outputfile);
			g_state.ulstate = CPP_COMMENT_STATE;
		}
		else
		{
			if(nextch == '*') //C
			{
				fputc(ch,g_state.outputfile);
			    fputc(nextch,g_state.outputfile);
			    g_state.ulstate = C_COMMENT_STATE;
			}
			else            //NO
			{
				fputc(ch,g_state.outputfile);
			}
		}
		break;
	case '\'':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '"')
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
			g_state.sg_flag = S_NO_FLAG;
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		break;
	case '"':
		fputc('"',g_state.outputfile);
		g_state.flag = NO_FLAG;
		g_state.ulstate = PERSIST_STATE;
		break;
	case EOF:
		g_state.ulstate = END_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
}

void EventProAtCpp(char ch)
{   
	char nextch;
    switch(ch)
	{
	case '/':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '/' || nextch == '*')                ////或///*             
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		break;
	case '*':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '/')
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		break;		
	case '\n':
		fputc('*',g_state.outputfile);
		fputc('/',g_state.outputfile);
		fputc('\n',g_state.outputfile);
		g_state.ulstate = NO_COMMENT_STATE;
		break;	
	case '"':
		fputc('"',g_state.outputfile);
		g_state.flag = CPP_FLAG;
		g_state.ulstate = PERSIST_STATE;
		break;
	case '\'':
		fputc(ch,g_state.outputfile);
		g_state.sg_flag = S_CPP_FLAG;
		g_state.ulstate = SINGLE_MARK;
		break;
	case EOF:
		fputc('*',g_state.outputfile);
		fputc('/',g_state.outputfile);
		g_state.ulstate = END_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);      
		break;
	}
}

void EventProAtC(char ch)
{
	char nextch;
	switch(ch)
	{
	case '*':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '/')
		{	
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
			g_state.ulstate = NO_COMMENT_STATE;	
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		break;
    case '/':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '/' || nextch == '*')
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		break;
	case '"':
		fputc('"',g_state.outputfile);
		g_state.flag = C_FLAG;
		g_state.ulstate = PERSIST_STATE;
		break;
    case '\'':
		fputc(ch,g_state.outputfile);
		g_state.sg_flag = S_C_FLAG;
		g_state.ulstate = SINGLE_MARK;
		break;
	case EOF:
		g_state.ulstate = END_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
}

void EventProAtPersist(char ch)               //双引号处理
{
	char nextch;
	switch(ch)
	{
	case '"':
		fputc('"',g_state.outputfile);
		switch(g_state.flag)
		{
		case NO_FLAG:
			g_state.ulstate = NO_COMMENT_STATE;
			break;
		case CPP_FLAG:
			g_state.ulstate = CPP_COMMENT_STATE;
			break;
		case C_FLAG:
			g_state.ulstate = C_COMMENT_STATE;
			break;
		}
		break;
	case '*':
		nextch = fgetc(g_state.inputfile);
		if('/' == nextch)
		{
			fputc(ch,g_state.outputfile);
			fputc(' ',g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
			char ag_nextch;
			ag_nextch = fgetc(g_state.inputfile);
			if(nextch == '*' && ag_nextch == '/')
			{
			    fputc(' ',g_state.outputfile);
			    fputc(ag_nextch,g_state.outputfile);
			}
		}
		break;
	case EOF:
		g_state.ulstate = END_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
}

void EventProAtSingle_Mark(char ch)
{
	switch(ch)
	{
	case EOF:
		g_state.ulstate = END_STATE;
		break;
	case '\'':
		fputc(ch,g_state.outputfile); 
		switch(g_state.sg_flag)
		{
		case S_NO_FLAG:
			g_state.ulstate = NO_COMMENT_STATE;
			break;
		case S_CPP_FLAG:
			g_state.ulstate = CPP_COMMENT_STATE;
			break;
		case S_C_FLAG:
			g_state.ulstate = C_COMMENT_STATE;
			break;
		}
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}

}

