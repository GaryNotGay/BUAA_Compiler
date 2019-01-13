#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD 4096 //��󵥴���
#define MAX_LINE 1024 //Դ������������
#define MAX_CHAR 1024 //Դ���������󳤶�
#define MAX_TOKEN 64 // �����ַ�������󳤶�
#define MAX_NUM 8 // ������ֵ����󳤶�
#define MAX_REMAIN 15 //������������
#define MAX_SYM 1024 //�����ű���
#define MAX_PCODE 4096 //���ָ������
#define MAX_OP 25 //����������
#define MAX_PCODE_LINE 4096
#define MAX_RUN 1024
#define MAX_STACK_NUM 1024

enum symbol
{
	CONSTSY = 1, INTSY, FLOATSY, CHARSY, VOIDSY, MAINSY, IFSY, ELSESY, WHILESY, SWITCHSY, CASESY, DEFAULTSY, SCANFSY, PRINTFSY, RETURNSY, //������
	IDSY, INTNUMSY, REALSY, CHARWORDSY, STRINGSY, //�ַ�
	IFEQSY, EQSY, NOEQSY, LTRISY, LTRIEQSY, RTRISY, RTRIEQSY, //˫���ж�
	PLUSSY, MINSY, STARSY, DIVSY, COMMASY, SEMISY, COLONSY, //һ���ж�
	SLPARSY, SRPARSY, MLPARSY, MRPARSY, LLPARSY, LRPARSY, //����
};

enum pcodeop
{
	LDI, /*������װ��ջ��*/ LDA, /*���ص�ַ*/ LOD, /*װ��*/ IXX, /*ȡ�±��ַ*/ STO, /*��ջ�����ݴ�����ջ���θ�ԪΪ��ַ�ĵ�Ԫ*/
	JPC, /*����Ϊ����ת*/ JMP, /*������ת*/
	RED, /*��*/ WRR, /*д*/
	INT, /*��ջ����ֵ���*/ ADD, /*+*/ SUB, /*-*/ MUL, /***/ DIV, /*/*/ MUS, /*-*/
	GRT, /*>*/ LES, /*<*/ EQL, /*==*/ NEQ, /*!=*/ GEQ, /*>=*/ LER, /*<=*/
	CAL, /*����*/  RET, /*����*/ EXF, /*�˳�*/ INF /*����*/
};

char *remainSymbol[MAX_REMAIN] = //������
{
	"const", "int", "float", "char", "void", "main", "if", "else", "while", "switch", "case", "default", "scanf", "printf", "return",
};

char *pcode_op[MAX_OP] =
{
	"LDI", "LDA", "LOD", "IXX", "STO",
	"JPC", "JMP",
	"RED", "WRR",
	"INT", "ADD", "SUB", "MUL", "DIV", "MUS",
	"GRT", "LES", "EQL", "NEQ", "GEQ", "LER",
	"CAL", "RET", "EXF", "INF"
};

struct word
{
	char name[100];
	int value;
	float value_real;
	int sym;
	int line;
}word[MAX_WORD];

struct pcode_inter
{
	int op;
	int lev;
	float num;
}pcode_inter[MAX_PCODE_LINE];

struct symbol_table
{
	char name[100]; //����
	int valid; //��Чλ
			   // 0 -- ��Ч   1 -- ��Ч
	int kind; //����
			  // 0 -- ����   1 -- ����   2 -- ����   3 -- �з��غ���   4 -- �޷��غ���   5 -- ������    6 -- ����
	int type; //����
			  // 0 -- ����   1 -- ʵ��   2 -- �ַ�
	int value; //��ֵ
	float value_real; //ʵ��
	int array[1024]; //����
	float array_real[1024]; //ʵ������
	int level; //���
	int line; //�к�
	int addr;
}sym_table[MAX_SYM];

struct pcode
{
	int op;
	int lev;
	int x;
	float y;
	char z;
}pcode[MAX_PCODE];

FILE *fp1, *fp2, *fp3;
enum symbol sym;
enum pcodeop op;
char char_in[MAX_LINE][MAX_CHAR]; //���Դ����
char char_token[MAX_TOKEN]; //��ű�ʶ��
char char_num[MAX_NUM]; //����޷�������
char ch; //��ǰ�ַ�
char code_addr[32]; //Դ����·��
int print_number; //ת��������
float print_number_real; //ת����ʵ��
int num = 1; //�ʷ�����ָ��
int word_num = 0; //�﷨����ָ��
int sym_num = 0; //���ű����ָ��
int sym_array[100]; //���ű�����
int array_num = 0; //����ָ��
int pcode_num = 1; //Pcode����ָ��
int line = 0; //��ָ��
int number = 0; //����ָ��
int current_level = 0; //���
int var_level[10]; //����ջ
int level = 0; //����ջ���
int case_array[10];
int case_num; //case���ָ��
int used_sym_num = 0; //�������
int para_count = 0; //��������
int para_label_num = 0;
int i; //��ʱ����
int error_flag = 0;
int case_pcode = 0;
int key_num = 0;
int print_type = 0; // 0 -- �ַ� 1 -- ���� 2 -- ���� 
int return_symbol = 0;
int temp_num_case = 0;
int char2int_flag = 0;

/*.....��������.....*/

//�ʷ�����
void getsym();
void getNewchar();

//�﷨����
void program();
void const_declare();
void var_declare();
void return_function();
void void_function();
void main_function();
void const_define();
void var_define();
void parameter_table();
void complex_statment();
void statment_list();
void statment();
void ifstatment();
void whilestatment();
void casestatment();
void scanfstatment();
void printfstatment();
void returnstatment();
void assignatatment();
void returnfuncstatment();
void voidfuncstatment();
void condition();
void case_table();
void defaultstatment();
void case_son();
void expression();
void term();
void factor();
void value_parameter_table();

//���ű����
void isFull();
void NewNode();
void CreateConstInt();
void FillConstInt();
void CreateConstFloat();
void FillConstFloat();
void CreateConstChar();
void FillConstChar();
void CreateInt();
void CreateIntArray();
void CreateFloat();
void CreateFloatArray();
void CreateChar();
void CreateCharArray();
void CreateIntFun();
void CreateFloatFun();
void CreateCharFun();
void CreateVoidFun();
void CreateMainFun();
void CreateIntPara();
void CreateFloatPara();
void CreateCharPara();
int isReDefine();
int Find(char name[]);

//��������
void gen_int(int op, int lev, int x);
void gen_float(int op, int lev, float x);
void gen_char(int op, int lev, char x);
void pcode_print();

//����ִ��
void Interpret();

//������
void Error(int e);

void main()
{
	printf("Please Enter The Code Address \n");
	scanf("%s", code_addr);

	fp1 = fopen(code_addr, "r");
	char chch;

	/*	for (i = 0; i < MAX_TOKEN; i++) //���Դ��������
	{
	char_in[i] = "\0";
	}*/

	/* for (i = 0; i < MAX_WORD; i++) //��ʼ�����ʽṹ
	{
	word[i].name[0] = '\0';
	word[i].line = 0;
	word[i].sym = 0;
	word[i].value = 0;
	word[i].value_real = 0;
	}*/

	for (i = 0; i < MAX_PCODE; i++) //��ʼ��Pcode����
	{
		pcode[i].op = 0;
		pcode[i].lev = 0;
		pcode[i].x = 0;
		pcode[i].y = 0;
		pcode[i].z = 0;
	}

	line = 0;
	number = 0;

	while ((chch = fgetc(fp1)) != EOF) //����Դ����
	{
		if (chch == '\n')
		{
			char_in[line][number] = chch;
			line++;
			number = 0;
		}
		char_in[line][number] = chch;
		number++;
	}
	fclose(fp1);
	char_in[line][number] = ' ';
	char_in[line][++number] = '\0';
	number = 0;
	line = 0;

	//�ʷ�����
	while (1)
	{
		getsym();
		if (sym >= 1 && sym <= MAX_REMAIN)
		{
			strcpy(word[word_num].name, remainSymbol[num - 1]);
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == IDSY)
		{
			strcpy(word[word_num].name, char_token);
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == INTNUMSY)
		{
			word[word_num].value = print_number;
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == REALSY)
		{
			word[word_num].value_real = print_number_real;
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == CHARWORDSY)
		{
			word[word_num].value = char_token[0];
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == STRINGSY)
		{
			strcpy(word[word_num].name, char_token);
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}

		else if (sym == IFEQSY)
		{
			//			strcpy(word[word_num].name, '==');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == EQSY)
		{
			//			strcpy(word[word_num].name, '=');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == NOEQSY)
		{
			//			strcpy(word[word_num].name, '!=');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == LTRISY)
		{
			//			strcpy(word[word_num].name, '<');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == LTRIEQSY)
		{
			//			strcpy(word[word_num].name, '<=');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == RTRISY)
		{
			//			strcpy(word[word_num].name, '>');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == RTRIEQSY)
		{
			//			strcpy(word[word_num].name, '>=');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}

		else if (sym == PLUSSY)
		{
			//			strcpy(word[word_num].name, '+');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == MINSY)
		{
			//			strcpy(word[word_num].name, '-');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == STARSY)
		{
			//			strcpy(word[word_num].name, '*');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == DIVSY)
		{
			//			strcpy(word[word_num].name, '/');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == COMMASY)
		{
			//			strcpy(word[word_num].name, ',');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == COLONSY)
		{
			//			strcpy(word[word_num].name, ':');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == SEMISY)
		{
			//			strcpy(word[word_num].name, ';');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}

		else if (sym == SLPARSY)
		{
			//			strcpy(word[word_num].name, '(');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == SRPARSY)
		{
			//			strcpy(word[word_num].name, ')');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == MLPARSY)
		{
			//			strcpy(word[word_num].name, '[');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == MRPARSY)
		{
			//			strcpy(word[word_num].name, ']');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == LLPARSY)
		{
			//			strcpy(word[word_num].name, '{');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}
		else if (sym == LRPARSY)
		{
			//			strcpy(word[word_num].name, '}');
			word[word_num].sym = sym;
			word[word_num].line = line;
			word_num++;
		}

		else if (ch == '\0')
		{
			//			printf("Done! \n");
			break;
		}
		else
		{
			printf("Error!\n");
			printf("%c Not Token\n", ch);
			//	break;
		}

	}

	printf("Token Analysis Success!\n");

	//�﷨���� && �������
	program();

	printf("Grammar Analysis Success!\n");

//	for (i = 0; i < sym_num; i++)
//		printf("%s %d %d %d\n", sym_table[i].name, sym_table[i].value, sym_table[i].level, sym_table[i].addr);

	getch();
	if (error_flag == 1)
		return;

	//��������
	pcode_print();

	printf("Pcode Product Success!\n");

	getch();

	//����ִ��
	Interpret();

	printf("Pcode Interpret Success!\n");
	
	getch();

	return;
}

// �ʷ�����

void getsym()
{
	int resultValue; //���ұ����ַ���ֵ
	char char_real[MAX_NUM];
	int alpha_number = 0;
	int real_number = 0;
	float print_num_small = 0;
	sym = 0;

	for (i = 0; i < MAX_TOKEN; i++) //���token����
	{
		char_token[i] = '\0';
	}

	for (i = 0; i < MAX_NUM; i++) //�����ֵ����
	{
		char_num[i] = '\0';
	}

	for (i = 0; i < MAX_NUM; i++) //���ʵ������
	{
		char_real[i] = '\0';
	}

	getNewchar();

	while (ch == ' ' || ch == '\n' || ch == '\t') //�ж��Ƿ�Ϊ�ո񡢻��С��Ʊ�
	{
		getNewchar();
	}

	if (isalpha(ch) != 0 || ch == '_') //�ж��Ƿ�Ϊ��ĸ  isalpha ��Ӣ��--0  ��д--1 Сд--2
	{

		while ((isalpha(ch) != 0) || (isdigit(ch) != 0) || ch == '_') //�ж���һ�ַ��Ƿ�Ϊ��ĸ������
		{
			char_token[alpha_number] = ch;
			alpha_number++;
			getNewchar();
		}

		number--;
		sym = IDSY;

		for (i = 0; i < MAX_TOKEN; i++) //��Сдת��
		{
			if (char_token[i] >= 65 && char_token[i] <= 90)
				char_token[i] = char_token[i] + 32;
		}
		for (i = 0; i < MAX_REMAIN; i++) //���ұ�����
		{
			if (stricmp(remainSymbol[i], char_token) == 0)
				sym = i + 1;
		}


	}

	else if (isdigit(ch) != 0) //�ж��Ƿ�Ϊ����
	{
		while ((isdigit(ch) != 0)) //�ж���һ�ַ��Ƿ�Ϊ����
		{
			char_num[alpha_number] = ch;
			alpha_number++;
			getNewchar();
		}
		if ((char_num[0] == '0') && (alpha_number != 1))
			Error(105); //������λ����Ϊ0
		if (ch == '.') //�ж��Ƿ�Ϊʵ��
		{
			getNewchar();
			while ((isdigit(ch) != 0)) //�ж���һ�ַ��Ƿ�Ϊ����
			{
				char_real[real_number] = ch;
				real_number++;
				getNewchar();
			}
			number--;
			print_num_small = atoi(char_real); //С������
			for (i = 0; i < real_number; i++)
			{
				print_num_small = print_num_small / 10;
			}
			print_number_real = atoi(char_num);
			print_number_real = print_number_real + print_num_small;
			sym = REALSY;
		}
		else
		{
			number--;
			print_number = atoi(char_num);
			sym = INTNUMSY;
		}

	}

	else if (ch == '=') //�ж��Ƿ�Ϊ�Ⱥ�
	{
		getNewchar();
		if (ch == '=') //�ж��Ƿ�Ϊ�жϷ���
			sym = IFEQSY;
		else
		{
			number--;
			sym = EQSY;
		}
	}

	else if (ch == '!') //�ж��Ƿ�Ϊ���Ⱥ�
	{
		getNewchar();
		if (ch == '=')
			sym = NOEQSY;
		else
		{
			number--;
			sym = 0;
			Error(101); //̾�ŵ�������
		}
	}

	else if (ch == '<')//�ж��Ƿ�ΪС�ں�
	{
		getNewchar();
		if (ch == '=')
			sym = LTRIEQSY;//�ж��Ƿ�ΪС�ڵ��ں�
		else
		{
			number--;
			sym = LTRISY;
		}
	}

	else if (ch == '>')//�ж��Ƿ�Ϊ���ں�
	{
		getNewchar();
		if (ch == '=')
			sym = RTRIEQSY;//�ж��Ƿ�Ϊ���ڵ��ں�
		else
		{
			number--;
			sym = RTRISY;
		}
	}


	else if (ch == '+') //�ж��Ƿ�Ϊ�Ӻ�
	{
		sym = PLUSSY;
	}

	else if (ch == '-') //�ж��Ƿ�Ϊ����
	{
		sym = MINSY;
	}

	else if (ch == '*') //�ж��Ƿ�Ϊ�Ǻ�
	{
		sym = STARSY;
	}

	else if (ch == '/') //�ж��Ƿ�Ϊ�Ǻ�
	{
		sym = DIVSY;
	}

	else if (ch == ',') //�ж��Ƿ�Ϊ����
	{
		sym = COMMASY;
	}

	else if (ch == ';') //�ж��Ƿ�Ϊ�ֺ�
	{
		sym = SEMISY;
	}

	else if (ch == ':') //�ж��Ƿ�Ϊð��
	{
		sym = COLONSY;
	}

	else if (ch == '(') //�ж��Ƿ�Ϊ������
	{
		sym = SLPARSY;
	}

	else if (ch == ')') //�ж��Ƿ�Ϊ������
	{
		sym = SRPARSY;
	}

	else if (ch == '[') //�ж��Ƿ�Ϊ������
	{
		sym = MLPARSY;
	}

	else if (ch == ']') //�ж��Ƿ�Ϊ������
	{
		sym = MRPARSY;
	}

	else if (ch == '{') //�ж��Ƿ�Ϊ������
	{
		sym = LLPARSY;
	}

	else if (ch == '}') //�ж��Ƿ�Ϊ������
	{
		sym = LRPARSY;
	}


	else if (ch == '\'') //�жϵ����ż������ַ�
	{
		getNewchar();
		if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || isalpha(ch) != 0 || isdigit(ch) != 0 || ch == '_') //<�ӷ������> <�˷������> <��ĸ> <����>
		{
			char_token[0] = ch;
			getNewchar();
			if (ch != '\'')
			{
				Error(102); //ȱ�ٺ�����
				number--;
			}
		}
		else
		{
			Error(103); //�����ڷ��ַ�
			char_token[0] = ch;
			number++;
		}

		sym = CHARWORDSY;
	}

	else if (ch == '"') //�ж�˫���ż������ַ�
	{
		getNewchar();
		while ((ch == 32 || ch == 33 || (ch >= 35 && ch <= 126)) && ch != '"')
		{
			char_token[alpha_number] = ch;
			alpha_number++;
			getNewchar();
		}

		if (ch != '"')
		{
			Error(102); //ȱ�ٺ�����
			number--;
		}

		sym = STRINGSY;
	}

}

void getNewchar() //getchar()
{
	if (number >= strlen(char_in[line]))
	{
		number = 0;
		line++;
	}
	ch = char_in[line][number];
	number++;

}

//�﷨����

void program()
{
	word_num = 0;
	array_num = 1;
	sym_array[0] = 0;

	if (word[word_num].sym == CONSTSY) //����˵��
		const_declare();

	if ((word[word_num].sym == INTSY || word[word_num].sym == FLOATSY || word[word_num].sym == CHARSY) && (word[word_num + 2].sym == COMMASY || word[word_num + 2].sym == SEMISY || word[word_num + 2].sym == MLPARSY)) //����˵��
																																																						// ��ǰ�ַ�Ϊ���ͱ�ʶ��int/float/char���ҵ���λӦΪ��/��/��
		var_declare();

	while (1)
	{
		return_symbol = 0;
		if ((word[word_num].sym == INTSY || word[word_num].sym == FLOATSY || word[word_num].sym == CHARSY) && (word[word_num + 2].sym == SLPARSY || word[word_num + 2].sym == LLPARSY)) //�з���ֵ��������
			return_function();
		else if (word[word_num].sym == VOIDSY && word[word_num + 1].sym != MAINSY) //�޷���ֵ��������
			void_function();
		else
			break;
	}

//	if(var_level[0] == 0)
//		gen_int(INF, 0, var_level[0]);
//	else
		gen_int(INF, 0, var_level[0] - 1);

	if (word[word_num].sym == VOIDSY && word[word_num + 1].sym == MAINSY)
		main_function();
	else
		Error(201); //ȱ��������

	return;
}

void const_declare() //����˵��
{
	if (word[word_num].sym != CONSTSY)
		Error(202); //Ӧ��const��ͷ

	const_define();
	if (word[word_num].sym != SEMISY)
		Error(203); //Ӧ�ԣ�����

	while (1)
	{
		word_num++;
		if (word[word_num].sym != CONSTSY)
			break;
		const_define();
//		printf("%d\n", word[word_num].sym);
		if (word[word_num].sym != SEMISY)
			Error(203); //Ӧ�ԣ�����
//		printf("%d\n", word[word_num].sym);
	}

	return;
}

void const_define()
{
	word_num++;

	if (word[word_num].sym == INTSY) //int����
	{
		word_num++;
		if (word[word_num].sym != IDSY)
			Error(204); //ӦΪ��ʶ��

		CreateConstInt();
		word_num++;
		if (word[word_num].sym != EQSY)
			Error(205); //ӦΪ=


		word_num++;
		if (word[word_num].sym != INTNUMSY && word[word_num].sym != PLUSSY && word[word_num].sym != MINSY)
			Error(206); //������


		if (word[word_num].sym == INTNUMSY || word[word_num].sym == PLUSSY || word[word_num].sym == MINSY)
		{
			if (word[word_num].sym == MINSY)
			{
				word_num++;
				if (word[word_num].sym == INTNUMSY)
				{
					word[word_num].value = word[word_num].value * -1;
					if (isReDefine() == 0)
						FillConstInt();
					else
						Error(302); //��ʶ���ض���
				}
				else
					Error(207); //���޷�������
			}
			else if (word[word_num].sym == PLUSSY)
			{
				word_num++;
				if (word[word_num].sym == INTNUMSY)
				{
					if (isReDefine() == 0)
						FillConstInt();
					else
						Error(302); //��ʶ���ض���
				}
				else
					Error(207); //���޷�������
			}
			else
			{
				if (word[word_num].sym == INTNUMSY)
				{
					if (isReDefine() == 0)
						FillConstInt();
					else
						Error(302); //��ʶ���ض���
				}
				else
					Error(207); //���޷�������
			}
		}

		while (1)
		{
			word_num++;
			if (word[word_num].sym != COMMASY)
				break;
			else
			{
				word_num++;
				if (word[word_num].sym != IDSY)
					Error(204); //ӦΪ��ʶ��

				CreateConstInt();
				word_num++;
				if (word[word_num].sym != EQSY)
					Error(205); //ӦΪ=

				word_num++;
				if (word[word_num].sym != INTNUMSY && word[word_num].sym != PLUSSY && word[word_num].sym != MINSY)
					Error(206); //������


				if (word[word_num].sym == INTNUMSY || word[word_num].sym == PLUSSY || word[word_num].sym == MINSY)
				{
					if (word[word_num].sym == MINSY)
					{
						word_num++;
						if (word[word_num].sym == INTNUMSY)
						{
							word[word_num].value = word[word_num].value * -1;
							if (isReDefine() == 0)
								FillConstInt();
							else
								Error(302); //��ʶ���ض���
						}
						else
							Error(207); //���޷�������
					}
					else if (word[word_num].sym == PLUSSY)
					{
						word_num++;
						if (word[word_num].sym == INTNUMSY)
						{
							if (isReDefine() == 0)
								FillConstInt();
							else
								Error(302); //��ʶ���ض���
						}
						else
							Error(207); //���޷�������
					}
					else if (word[word_num].sym == INTNUMSY)
					{
						if (isReDefine() == 0)
							FillConstInt();
						else
							Error(302); //��ʶ���ض���
					}
					else
						Error(207); //���޷�������
				}
			}
		}
	}

	else if (word[word_num].sym == FLOATSY) //float����
	{
		word_num++;
		if (word[word_num].sym != IDSY)
			Error(204); //ӦΪ��ʶ��

		CreateConstFloat();
		word_num++;
		if (word[word_num].sym != EQSY)
			Error(205); //ӦΪ=

		word_num++;
		if (word[word_num].sym != REALSY && word[word_num].sym != PLUSSY && word[word_num].sym != MINSY)
			Error(208); //��ʵ��

		if (word[word_num].sym == REALSY || word[word_num].sym == PLUSSY || word[word_num].sym == MINSY)
		{
			if (word[word_num].sym == MINSY)
			{
				word_num++;
				if (word[word_num].sym == REALSY)
				{
					word[word_num].value_real = word[word_num].value_real * -1;
					if (isReDefine() == 0)
						FillConstFloat();

					else
						Error(302); //��ʶ���ض���
				}
				else
					Error(207); //���޷�������

			}
			else if (word[word_num].sym == PLUSSY)
			{
				word_num++;
				if (word[word_num].sym == REALSY)
				{
					if (isReDefine() == 0)
						FillConstFloat();

					else
						Error(302); //��ʶ���ض���
				}
				else
					Error(208); //��ʵ��
			}
			else
			{
//				word_num++;
				if (word[word_num].sym == REALSY)
				{
					if (isReDefine() == 0)
						FillConstFloat();

					else
						Error(302); //��ʶ���ض���
				}
				else
					Error(208); //��ʵ��
			}
		}

		while (1)
		{
//			printf("%d\n", word[word_num].sym);
			word_num++;
//			printf("%d\n", word[word_num].sym);
			if (word[word_num].sym != COMMASY)
				break;
			else
			{
				word_num++;
				if (word[word_num].sym != IDSY)
					Error(204); //ӦΪ��ʶ��

				CreateConstFloat();
				word_num++;
				if (word[word_num].sym != EQSY)
					Error(205); //ӦΪ=

				word_num++;
				if (word[word_num].sym != REALSY && word[word_num].sym != PLUSSY && word[word_num].sym != MINSY)
					Error(208); //��ʵ��

				if (word[word_num].sym == REALSY || word[word_num].sym == PLUSSY || word[word_num].sym == MINSY)
				{
					if (word[word_num].sym == MINSY)
					{
						word_num++;
						if (word[word_num].sym == REALSY)
						{
							word[word_num].value_real = word[word_num].value_real * -1;
							if (isReDefine() == 0)
								FillConstFloat();

							else
								Error(302); //��ʶ���ض���
						}
						else
							Error(207); //���޷�������
					}
					else if (word[word_num].sym == PLUSSY)
					{
						word_num++;
						if (word[word_num].sym == REALSY)
						{
							if (isReDefine() == 0)
								FillConstFloat();

							else
								Error(302); //��ʶ���ض���
						}
						else
							Error(208); //��ʵ��
					}
					else
					{
						if (word[word_num].sym == REALSY)
						{
							if (isReDefine() == 0)
								FillConstFloat();
							else
								Error(302); //��ʶ���ض���
						}
						else
							Error(208); //��ʵ��
					}
				}
			}
		}

//		printf("%d\n", word[word_num].sym);
	}

	else if (word[word_num].sym == CHARSY)
	{
		word_num++;
		if (word[word_num].sym != IDSY)
			Error(204); //ӦΪ��ʶ��

		CreateConstChar();
		word_num++;
		if (word[word_num].sym != EQSY)
			Error(205); //ӦΪ=

		word_num++;
		if (word[word_num].sym != CHARWORDSY)
			Error(209); //���ַ�

		if (word[word_num].sym == CHARWORDSY)
		{
			if (isReDefine() == 0)
				FillConstChar();
			else
				Error(302); //��ʶ���ض���
		}

		while (1)
		{
			word_num++;
			if (word[word_num].sym != COMMASY)
				break;
			else
			{
				word_num++;
				if (word[word_num].sym != IDSY)
					Error(204); //ӦΪ��ʶ��

				CreateConstChar();
				word_num++;
				if (word[word_num].sym != EQSY)
					Error(205); //ӦΪ=

				word_num++;
				if (word[word_num].sym != CHARWORDSY && word[word_num].sym != PLUSSY && word[word_num].sym != MINSY)
					Error(209); //���ַ�

				if (word[word_num].sym == CHARWORDSY)
				{
					if (isReDefine() == 0)
						FillConstChar();
					else
						Error(302); //��ʶ���ض���
				}

			}
		}
	}

	else
		Error(210); //�����ͱ�ʶ��

	return;
}

void var_declare()
{
	var_define();
	if (word[word_num].sym != SEMISY)
		Error(203);

	word_num++;
	while (1)
	{
		if ((word[word_num].sym == INTSY || word[word_num].sym == FLOATSY || word[word_num].sym == CHARSY) && (word[word_num + 2].sym == COMMASY || word[word_num + 2].sym == SEMISY || word[word_num + 2].sym == MLPARSY))
		{
//			printf("%s %d\n", word[word_num + 1].name, word[word_num + 2].sym);
			var_define();
			if (word[word_num].sym != SEMISY)
				Error(203);
		}
		else
			break;
		word_num++;
	}
	if (level != 0)
		gen_int(INF, var_level[level] - 1, 0);
	return;
}

void var_define()
{
	int flag = 0; //int=1,float=2,char=3
	if (word[word_num].sym != INTSY && word[word_num].sym != FLOATSY && word[word_num].sym != CHARSY)
		Error(210);

	if (word[word_num].sym == INTSY)
		flag = 1;
	else if (word[word_num].sym == FLOATSY)
		flag = 2;
	else if (word[word_num].sym == CHARSY)
		flag = 3;

var_define_label:
	word_num++;
	if (word[word_num].sym != IDSY)
		Error(204);
	else
	{
//		printf("%s\n", word[word_num].name);
		word_num++;
		if (word[word_num].sym != COMMASY && word[word_num].sym != MLPARSY && word[word_num].sym != SEMISY)
			Error(211); //��ʽ���Ϸ�

		else if (word[word_num].sym == COMMASY)
		{
			if (flag == 1) //int
			{
				word_num--;
				if (isReDefine() == 0)
				{
					CreateInt();
					sym_num--;
					sym_table[sym_num].addr = var_level[level];
					var_level[level]++;
					strcpy(sym_table[sym_num].name, word[word_num].name);
//					printf("%s, %d\n", sym_table[sym_num].name, sym_table[sym_num].level);
					sym_num++;
				}
				else
					Error(302); //��ʶ���ض���
				word_num++;
			}
			else if (flag == 2) //float
			{
				word_num--;
				if (isReDefine() == 0)
				{
					CreateFloat();
					sym_num--;
					sym_table[sym_num].addr = var_level[level];
					var_level[level]++;
					strcpy(sym_table[sym_num].name, word[word_num].name);
					sym_num++;
				}
				else
					Error(302); //��ʶ���ض���
				word_num++;
			}
			else if (flag == 3) //char
			{
				word_num--;
				if (isReDefine() == 0)
				{
					CreateChar();
					sym_num--;
					sym_table[sym_num].addr = var_level[level];
					var_level[level]++;
					strcpy(sym_table[sym_num].name, word[word_num].name);
					sym_num++;
				}
				else
					Error(302); //��ʶ���ض���
				word_num++;
			}
			goto var_define_label;
		}

		else if (word[word_num].sym == MLPARSY)
		{
			word_num++;
			if (word[word_num].sym != INTNUMSY)
				Error(207); //��ʽ���Ϸ�

			word_num++;
			if (word[word_num].sym != MRPARSY)
				Error(212); //ȱ�ٺ�����

			if (flag == 1) //int
			{
				CreateIntArray();
				sym_num--;
				sym_table[sym_num].addr = var_level[level];
				var_level[level]++;
				sym_table[sym_num].value = word[word_num - 1].value;
				//				sym_table[sym_num].addr = var_level[level];
				var_level[level] = var_level[level] + sym_table[sym_num].value;
				strcpy(sym_table[sym_num].name, word[word_num - 3].name);
//				printf("%s %d %d\n", sym_table[sym_num].name, var_level[level], sym_table[sym_num].value);
				sym_num++;
			}
			else if (flag == 2) //float
			{
				CreateFloatArray();
				sym_num--;
				sym_table[sym_num].addr = var_level[level];
				var_level[level]++;
				sym_table[sym_num].value_real = word[word_num - 1].value_real;
				//				sym_table[sym_num].addr = var_level[level];
				var_level[level] = var_level[level] + sym_table[sym_num].value;
				strcpy(sym_table[sym_num].name, word[word_num - 3].name);
				sym_num++;
			}
			else if (flag == 3) //char
			{
				CreateCharArray();
				sym_num--;
				sym_table[sym_num].addr = var_level[level];
				var_level[level]++;
				sym_table[sym_num].value = word[word_num - 1].value;
				//				sym_table[sym_num].addr = var_level[level];
				var_level[level] = var_level[level] + sym_table[sym_num].value;
				strcpy(sym_table[sym_num].name, word[word_num - 3].name);
				sym_num++;
			}

			word_num++;
			if (word[word_num].sym == COMMASY)
				goto var_define_label;
		}

		else
		{
			word_num--;
			if (flag == 1) //int
			{
				CreateInt();
				sym_num--;
				sym_table[sym_num].addr = var_level[level];
				var_level[level]++;
				strcpy(sym_table[sym_num].name, word[word_num].name);
				sym_num++;
			}
			else if (flag == 2) //float
			{
				CreateFloat();
				sym_num--;
				sym_table[sym_num].addr = var_level[level];
				var_level[level]++;
				strcpy(sym_table[sym_num].name, word[word_num].name);
				sym_num++;
			}
			else if (flag == 3) //char
			{
				CreateChar();
				sym_num--;
				sym_table[sym_num].addr = var_level[level];
				var_level[level]++;
				strcpy(sym_table[sym_num].name, word[word_num].name);
				sym_num++;
			}
			word_num++;
		}
	}

	return;
}

void return_function()
{
	int flag = 0; //int=1,float=2,char=3
	para_count = 0;
	return_symbol = 1;

	if (word[word_num].sym != INTSY && word[word_num].sym != FLOATSY && word[word_num].sym != CHARSY)
		Error(210);

	if (word[word_num].sym == INTSY)
		flag = 1;
	else if (word[word_num].sym == FLOATSY)
		flag = 2;
	else if (word[word_num].sym == CHARSY)
		flag = 3;

	word_num++;
	if (word[word_num].sym != IDSY)
		Error(204);

	if (isReDefine() == 0)
	{
		if (flag == 1)
			CreateIntFun();
		else if (flag == 2)
			CreateFloatFun();
		else if (flag == 3)
			CreateCharFun();
	}
	else
		Error(302); //��ʶ���ض���

	current_level = 1;
	level++;
	word_num++;
	if (word[word_num].sym == SLPARSY)
	{
		word_num++;
		parameter_table();
		if (word[word_num].sym != SRPARSY)
			Error(212);
	}

	//	gen_int(INF, 0, para_count);
	sym_table[used_sym_num].value = para_count;
	word_num++;
	if (word[word_num].sym == LLPARSY)
	{
		word_num++;
		complex_statment();
		if (word[word_num].sym != LRPARSY)
			Error(212);
	}

	if (pcode[pcode_num - 1].op != EXF)
	{
		pcode[pcode_num].op = EXF;
		pcode[pcode_num].lev = 0;
		pcode[pcode_num].x = 0;
		pcode_num++;
	}

	var_level[level] = 0;
	level--;
	//	gen_int(EXF, 0, 0);
	current_level = 0;
	word_num++;
	return;
}

void void_function()
{
	para_count = 0;
	return_symbol = 1;

	if (word[word_num].sym != VOIDSY)
		Error(213);

	word_num++;
	if (word[word_num].sym != IDSY)
		Error(204);

	if (isReDefine() == 0)
		CreateVoidFun();
	else
		Error(302); //��ʶ���ض���

	current_level = 1;
	level++;
	word_num++;
	if (word[word_num].sym == SLPARSY)
	{
		word_num++;
		parameter_table();
		if (word[word_num].sym != SRPARSY)
			Error(212);
	}

	//	gen_int(INF, 0, para_count);
	sym_table[used_sym_num].value = para_count;

	word_num++;
	if (word[word_num].sym == LLPARSY)
	{
		word_num++;
		complex_statment();
		if (word[word_num].sym != LRPARSY)
			Error(212);
	}

	if (pcode[pcode_num - 1].op != EXF)
	{
		pcode[pcode_num].op = EXF;
		pcode[pcode_num].lev = 0;
		pcode[pcode_num].x = 0;
		pcode_num++;
	}
	var_level[level] = 0;
	level--;
	//	gen_int(EXF, 0, 0);
	current_level = 0;
	word_num++;
	return;
}

void parameter_table()
{
	int flag = 0;
	para_count = 0;

	if (word[word_num].sym == SRPARSY)
		goto parameter_table_end;

	if (word[word_num].sym != INTSY && word[word_num].sym != FLOATSY && word[word_num].sym != CHARSY)
		Error(210);

	if (word[word_num].sym == INTSY)
		flag = 1;
	else if (word[word_num].sym == FLOATSY)
		flag = 2;
	else if (word[word_num].sym == CHARSY)
		flag = 3;

	word_num++;

	if (word[word_num].sym != IDSY)
		Error(204);

	else
	{
		if (flag == 1)
		{
			CreateIntPara();
			sym_num--;
			sym_table[sym_num].addr = var_level[level];
			var_level[level]++;
			strcpy(sym_table[sym_num].name, word[word_num].name);
			sym_num++;
		}

		else if (flag == 2)
		{
			CreateFloatPara();
			sym_num--;
			sym_table[sym_num].addr = var_level[level];
			var_level[level]++;
			strcpy(sym_table[sym_num].name, word[word_num].name);
			sym_num++;
		}

		else if (flag == 3)
		{
			CreateCharPara();
			sym_num--;
			sym_table[sym_num].addr = var_level[level];
			var_level[level]++;
			strcpy(sym_table[sym_num].name, word[word_num].name);
			sym_num++;
		}
	}

	while (1)
	{
		word_num++;
		if (word[word_num].sym != COMMASY)
			break;
		else
		{
			word_num++;
			if (word[word_num].sym != INTSY && word[word_num].sym != FLOATSY && word[word_num].sym != CHARSY)
				Error(210);

			if (word[word_num].sym == INTSY)
				flag = 1;
			else if (word[word_num].sym == FLOATSY)
				flag = 2;
			else if (word[word_num].sym == CHARSY)
				flag = 3;

			word_num++;
			if (word[word_num].sym != IDSY)
				Error(204);

			else
			{
				if (flag == 1)
				{
					CreateIntPara();
					sym_num--;
					sym_table[sym_num].addr = var_level[level];
					var_level[level]++;
					strcpy(sym_table[sym_num].name, word[word_num].name);
					sym_num++;
				}

				else if (flag == 2)
				{
					CreateFloatPara();
					sym_num--;
					sym_table[sym_num].addr = var_level[level];
					var_level[level]++;
					strcpy(sym_table[sym_num].name, word[word_num].name);
					sym_num++;
				}

				else if (flag == 3)
				{
					CreateCharPara();
					sym_num--;
					sym_table[sym_num].addr = var_level[level];
					var_level[level]++;
					strcpy(sym_table[sym_num].name, word[word_num].name);
					sym_num++;
				}
			}
		}
	}

parameter_table_end:
	return;
}

void complex_statment()
{
	if (word[word_num].sym == CONSTSY)
		const_declare();

	if ((word[word_num].sym == INTSY || word[word_num].sym == FLOATSY || word[word_num].sym == CHARSY) && (word[word_num + 2].sym == COMMASY || word[word_num + 2].sym == SEMISY || word[word_num + 2].sym == MLPARSY))
		var_declare();

	statment_list();

	return;
}

void statment_list()
{
	while (1)
	{
		if (
			(word[word_num].sym == IFSY) || //if���
			(word[word_num].sym == WHILESY) || //while���
			(word[word_num].sym == SCANFSY) || //scanf���
			(word[word_num].sym == PRINTFSY) ||  //printf���
			(word[word_num].sym == SEMISY) || //�����
			(word[word_num].sym == SWITCHSY) || //������
			(word[word_num].sym == RETURNSY) || //�������
			((word[word_num].sym == IDSY) && (word[word_num + 1].sym == EQSY || word[word_num + 1].sym == MLPARSY)) || //��ֵ���
			(word[word_num].sym == IDSY) //���غ����������
			)
			statment();
		else
			break;
	}

	return;
}

void statment()
{
	if (word[word_num].sym == IFSY)
		ifstatment();

	else if (word[word_num].sym == WHILESY)
		whilestatment();

	else if (word[word_num].sym == SWITCHSY)
		casestatment();

	else if (word[word_num].sym == SEMISY)
		word_num++;

	else if (word[word_num].sym == SCANFSY)
	{
		scanfstatment();
		if (word[word_num].sym != SEMISY)
			Error(203);
		word_num++;
	}

	else if (word[word_num].sym == PRINTFSY)
	{
		printfstatment();
		if (word[word_num].sym != SEMISY)
			Error(203);
		word_num++;
	}

	else if (word[word_num].sym == RETURNSY)
	{
		returnstatment();
		if (word[word_num].sym != SEMISY)
			Error(203);
		word_num++;
	}

	else if ((word[word_num].sym == IDSY) && (word[word_num + 1].sym == EQSY || word[word_num + 1].sym == MLPARSY))
	{
		assignatatment();
		if (word[word_num].sym != SEMISY)
			Error(203);
		word_num++;
	}

	else if (word[word_num].sym == IDSY)
	{
		int flag = 0; //0 -- Ĭ�� 1 -- �з��� 2 -- �޷��� 
		int j = 0;

		if (current_level == 0)
		{
			for (j = sym_num; j >= 0; j--)
			{
				if (sym_table[j].valid == 0)
					continue;

				if (sym_table[j].level != 0)
					continue;

				if (strcmp(sym_table[j].name, word[word_num].name) == 0)
				{
					if (sym_table[j].kind == 3)
						flag = 1;

					if (sym_table[j].kind == 4)
						flag = 2;

					break;
				}
			}
		}

		if (current_level == 1)
		{
			for (j = sym_num; j >= 0; j--)
			{
				if (sym_table[j].valid == 0)
					continue;

				if (strcmp(sym_table[j].name, word[word_num].name) == 0)
				{
					if (sym_table[j].kind == 3)
						flag = 1;

					if (sym_table[j].kind == 4)
						flag = 2;

					break;
				}
			}
		}

		if (flag == 1) //�з���
			returnfuncstatment();
		if (flag == 2) //�޷���
			voidfuncstatment();
		if (word[word_num].sym != SEMISY)
			Error(203);
		word_num++;
	}

	else if (word[word_num].sym == LLPARSY)
	{
		word_num++;
		statment_list();
		if (word[word_num].sym != LRPARSY)
			Error(212);
		word_num++;
	}

	else
		Error(214);

	return;
}

void ifstatment()
{
	int temp_num1 = 0;
	int temp_num2 = 0;

	if (word[word_num].sym != IFSY)
		Error(215);

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);

	word_num++;
	condition();

	if (word[word_num].sym != SRPARSY)
		Error(212);

	temp_num1 = pcode_num;
	gen_int(JPC, 0, 0);

	word_num++;
	statment();

	if (word[word_num].sym == ELSESY)
	{
		temp_num2 = pcode_num;
		gen_int(JMP, 0, 0);
		pcode[temp_num1].x = pcode_num;
		word_num++;
		statment();
		pcode[temp_num2].x = pcode_num;
	}
	else
		pcode[temp_num1].x = pcode_num;

	return;
}

void condition()
{
	int double_expr = 0;
	int j = 0;
	expression();

	if (word[word_num].sym == IFEQSY || word[word_num].sym == NOEQSY || word[word_num].sym == LTRISY || word[word_num].sym == RTRISY || word[word_num].sym == LTRIEQSY || word[word_num].sym == RTRIEQSY)
	{
		double_expr = 1;
		switch (word[word_num].sym)
		{
		case IFEQSY: j = 1; break;
		case NOEQSY: j = 2; break;
		case LTRISY: j = 3; break;
		case RTRISY: j = 4; break;
		case LTRIEQSY: j = 5; break;
		case RTRIEQSY: j = 6; break;
		default: j = 0;
		}

		word_num++;
		expression();
	}

	if (double_expr == 1)
	{
		switch (j)
		{
		case 1: gen_int(EQL, 0, 0); break;
		case 2: gen_int(NEQ, 0, 0); break;
		case 3: gen_int(LES, 0, 0); break;
		case 4: gen_int(GRT, 0, 0); break;
		case 5: gen_int(LER, 0, 0); break;
		case 6: gen_int(GEQ, 0, 0); break;
		}
	}

	return;
}

void whilestatment()
{
	int temp_num1 = 0;
	int temp_num2 = 0;
	if (word[word_num].sym != WHILESY)
		Error(216);

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);

	word_num++;
	temp_num1 = pcode_num;
	condition();
	temp_num2 = pcode_num;
	gen_int(JPC, 0, 0);

	if (word[word_num].sym != SRPARSY)
		Error(212);

	word_num++;
	statment();
	gen_int(JMP, 0, temp_num1);
	pcode[temp_num2].x = pcode_num;

	return;
}

void casestatment()
{
	int j = 0;

	if (word[word_num].sym != SWITCHSY)
		Error(217);

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);

	word_num++;
	expression();
	case_pcode = pcode_num - 1;

	if (word[word_num].sym != SRPARSY)
		Error(212);

	word_num++;
	if (word[word_num].sym != LLPARSY)
		Error(211);

	word_num++;
	case_table();

	if (word[word_num].sym == DEFAULTSY)
		defaultstatment();

	if (word[word_num].sym != LRPARSY)
		Error(212);

	for (j = 0; j < case_num; j++)
		pcode[case_array[j]].x = pcode_num;

	word_num++;
	return;
}

void case_table()
{
	case_son();

	while (1)
	{
		pcode[temp_num_case].x = pcode_num;
		if (word[word_num].sym != CASESY)
			break;
		else
		{
			pcode[pcode_num].op = pcode[case_pcode].op;
			pcode[pcode_num].lev = pcode[case_pcode].lev;
			pcode[pcode_num].x = pcode[case_pcode].x;
			pcode[pcode_num].y = pcode[case_pcode].y;
			pcode[pcode_num].z = pcode[case_pcode].z;
			pcode_num++;
			case_son();
		}

	}

	return;
}

void case_son()
{
	if (word[word_num].sym != CASESY)
		Error(218);

	word_num++;
	if (word[word_num].sym == PLUSSY || word[word_num].sym == MINSY || word[word_num].sym == INTNUMSY || word[word_num].sym == CHARWORDSY)
	{
		if ((word[word_num].sym == MINSY) && (word[word_num + 1].sym == INTNUMSY))
		{
			word_num++;
			word[word_num].value = word[word_num].value * -1;
			gen_int(LDI, 0, word[word_num].value);
			gen_int(EQL, 0, 0);
			temp_num_case = pcode_num;
			gen_int(JPC, 0, 0);
		}

		if (((word[word_num].sym == PLUSSY) && (word[word_num + 1].sym == INTNUMSY)) || (word[word_num].sym == INTNUMSY))
		{
			if (word[word_num].sym == PLUSSY)
				word_num++;
			gen_int(LDI, 0, word[word_num].value);
			gen_int(EQL, 0, 0);
			temp_num_case = pcode_num;
			gen_int(JPC, 0, 0);
		}

		if (word[word_num].sym == CHARWORDSY)
		{
			gen_char(LDI, 0, word[word_num].value);
			gen_int(EQL, 0, 0);
			temp_num_case = pcode_num;
			gen_int(JPC, 0, 0);
		}

	}
	else
		Error(219);

	word_num++;
	if (word[word_num].sym != COLONSY)
		Error(220);

	word_num++;
	statment();
	case_array[case_num] = pcode_num;
	case_num++;
	gen_int(JMP, 0, 0);

	return;
}

void defaultstatment()
{
	if (word[word_num].sym != DEFAULTSY)
		Error(221);

	word_num++;
	if (word[word_num].sym != COLONSY)
		Error(220);

	word_num++;
	statment();

	case_array[case_num] = pcode_num;
	case_num++;
	gen_int(JMP, 0, 0);

	return;
}

void expression()
{
	int flag = 0;
	if (word[word_num].sym == PLUSSY || word[word_num].sym == MINSY)
	{
		if (word[word_num].sym == MINSY)
			flag = 1;
		word_num++;
	}

	term();
	if (flag == 1)
		gen_int(MUS, 0, 0);

	while (1)
	{
		if (word[word_num].sym != PLUSSY && word[word_num].sym != MINSY)
			break;
		else
		{
			print_type = 1;
			if (word[word_num].sym == PLUSSY)
			{
				word_num++;
				term();
				gen_int(ADD, 0, 0);
			}
			else
			{
				word_num++;
				term();
				gen_int(SUB, 0, 0);
			}
		}
	}

	return;
}

void term()
{
	factor();

	while (1)
	{
		if (word[word_num].sym != STARSY && word[word_num].sym != DIVSY)
			break;
		else
		{
			print_type = 1;
			if (word[word_num].sym == STARSY)
			{
				word_num++;
				factor();
				gen_int(MUL, 0, 0);
			}
			else
			{
				word_num++;
				factor();
				gen_int(DIV, 0, 0);
			}
		}
	}

	return;
}

void factor()
{
	int flag1 = 0;

	if (word[word_num].sym == IDSY)
	{
		if (word[word_num + 1].sym == MLPARSY)
		{
			int flag = 0;
			int array_level;
			flag = Find(word[word_num].name);
			if (sym_table[flag].type == 0) //����
				print_type = 1;
			if (sym_table[flag].type == 1) //ʵ��
				print_type = 2;
			if (sym_table[flag].type == 2) //�ַ�
				print_type = 0;
			word_num++;
			word_num++;
			if (word[word_num].sym == INTNUMSY)
			{
//				flag = Find(word[word_num - 2].name);
				if (word[word_num].value > sym_table[flag].value)
					Error(227); //����Խ��
			}
			else
			{
				flag1 = Find(word[word_num].name);
//				flag = Find(word[word_num - 2].name);
				if (sym_table[flag1].kind == 0)
				{
					if (sym_table[flag1].value > sym_table[flag].value)
						Error(227); //����Խ��
				}
			}

			expression();

			gen_int(IXX, sym_table[flag].level , sym_table[flag].addr);
//			gen_int(IXX, 1, sym_table[flag].addr);
			gen_int(LOD, 0, -1);
			if (word[word_num].sym != MRPARSY)
				Error(212);
			else
				word_num++;
		}

		else if (word[word_num + 1].sym == SLPARSY)//�з���ֵ�����������
		{
			int flag = 0;
			flag = Find(word[word_num].name);
			if (sym_table[flag].type == 0) //����
				print_type = 1;
			if (sym_table[flag].type == 1) //ʵ��
				print_type = 2;
			if (sym_table[flag].type == 2) //�ַ�
				print_type = 0;
			returnfuncstatment();
		}

		else //��ʶ��
		{
			int flag = 0;
			flag = Find(word[word_num].name);
			if (flag == -1)
				Error(401);
			if (sym_table[flag].type == 0) //����
				print_type = 1;
			if (sym_table[flag].type == 1) //ʵ��
				print_type = 2;
			if (sym_table[flag].type == 2) //�ַ�
				print_type = 0;

			if (sym_table[flag].kind == 0) //����
			{
				if (sym_table[flag].type == 0) //����
					gen_int(LDI, 1, sym_table[flag].value);
				else if (sym_table[flag].type == 1) //ʵ��
					gen_float(LDI, 2, sym_table[flag].value_real);
				else if (sym_table[flag].type == 2) //�ַ�
					gen_char(LDI, 0, sym_table[flag].value);
			}

			else if (sym_table[flag].kind == 1) //����
			{
//				printf("%d %d\n", sym_table[flag].type, sym_table[flag].addr);
				if (sym_table[flag].type == 0) //����
					gen_int(LOD, sym_table[flag].level, sym_table[flag].addr);
				else if (sym_table[flag].type == 1) //ʵ��
					gen_int(LOD, sym_table[flag].level, sym_table[flag].addr);
				else if (sym_table[flag].type == 2) //�ַ�
					gen_char(LOD, sym_table[flag].level, sym_table[flag].addr);
			}

			else if (sym_table[flag].kind == 6) //����
			{
				if (sym_table[flag].type == 0) //����
					gen_int(LOD, sym_table[flag].level, sym_table[flag].addr);
				else if (sym_table[flag].type == 1) //ʵ��
					gen_int(LOD, sym_table[flag].level, sym_table[flag].addr);
				else if (sym_table[flag].type == 2) //�ַ�
					gen_char(LOD, sym_table[flag].level, sym_table[flag].addr);
			}

			word_num++;
		}

	}

	else if (((word[word_num].sym == PLUSSY || word[word_num].sym == MINSY) && word[word_num + 1].sym == INTNUMSY) || word[word_num].sym == INTNUMSY)
	{
		print_type = 1; //����
		if (word[word_num].sym == MINSY)
		{
			word_num++;
			word[word_num].value = word[word_num].value * -1;
			gen_int(LDI, 0, word[word_num].value);
		}
		else if (word[word_num].sym == PLUSSY)
		{
			word_num++;
			gen_int(LDI, 0, word[word_num].value);
		}
		else
			gen_int(LDI, 0, word[word_num].value);
		word_num++;
	}

	else if (((word[word_num].sym == PLUSSY || word[word_num].sym == MINSY) && word[word_num + 1].sym == REALSY) || word[word_num].sym == REALSY)
	{
		print_type = 2; //����
		if (word[word_num].sym == MINSY)
		{
			word_num++;
			word[word_num].value_real = word[word_num].value_real * -1;
			gen_float(LOD, 0, word[word_num].value_real);
		}
		else if (word[word_num].sym == PLUSSY)
		{
			word_num++;
			gen_float(LOD, 0, word[word_num].value_real);
		}
		else
		{
			gen_float(LOD, 0, word[word_num].value_real);
		}
		word_num++;
	}

	else if (word[word_num].sym == CHARWORDSY)
	{
		print_type = 0; //�ַ�
		word_num++;
		gen_char(LOD, 0, word[word_num].name);

	}

	else if (word[word_num].sym == SLPARSY)
	{
		word_num++;
		char2int_flag = 1; 
		expression();
		print_type = 1;
		if (word[word_num].sym != SRPARSY)
		{
			Error(212);
//			printf("%d\n", word[word_num].sym);
		}
		word_num++;
	}

	else
		Error(211);

	return;
}

void scanfstatment()
{
	int flag = 0;

	if (word[word_num].sym != SCANFSY)
		Error(222);

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);

	word_num++;
	if (word[word_num].sym != IDSY)
		Error(204);
	else
	{
		flag = Find(word[word_num].name);
		if (flag == -1)
			Error(401);

		else if (sym_table[flag].kind == 0)
			Error(402);

		else
		{
			gen_int(RED, 0, 0);
			if (sym_table[flag].level == 0)
				gen_int(LDA, 0, sym_table[flag].addr);
			else if (sym_table[flag].level == 1)
				gen_int(LDA, 1, sym_table[flag].addr);
			gen_int(STO, 0, 0);
		}

	}

	word_num++;
	while (1)
	{
		if (word[word_num].sym != COMMASY)
			break;
		else
		{
			word_num++;
			if (word[word_num].sym != IDSY)
				Error(204);
			else
			{
				flag = Find(word[word_num].name);
				if (flag == -1)
					Error(401);

				else if (sym_table[flag].kind == 0)
					Error(402);

				else
				{
					gen_int(RED, 0, 0);
					if (sym_table[flag].kind == 1) //����
						gen_int(LDA, 0, sym_table[flag].addr);
					else if (sym_table[flag].kind == 6) //����
						gen_int(LDA, sym_table[flag].level, sym_table[flag].addr);
					gen_int(STO, 0, 0);
				}
			}
		}
		word_num++;
	}

	if (word[word_num].sym == SRPARSY)
		word_num++;

	return;
}

void printfstatment()
{
	// 0 -- �ַ� 1 -- ���� 2 -- ���� 
	int j = 0;

	if (word[word_num].sym != PRINTFSY)
		Error(223);

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);

	word_num++;
	if (word[word_num].sym == STRINGSY)
	{
		for (j = 0; j < strlen(word[word_num].name); j++)
			gen_char(WRR, 0, word[word_num].name[j]);

		gen_int(WRR, 0, -1);
		word_num++;
		if (word[word_num].sym == COMMASY)
		{
			pcode_num--;
			word_num++;
			expression();
			gen_int(WRR, print_type, 0);
			gen_int(WRR, 0, -1);
		}

		if (word[word_num].sym != SRPARSY)
		{
			Error(212);
//			printf("%d\n", word[word_num].sym);
		}
			
		word_num++;
	}

	else
	{
		expression();
		gen_int(WRR, print_type, 0);
		gen_int(WRR, 0, -1);

		if (word[word_num].sym != SRPARSY)
		{
			Error(212);
			printf("%d\n", word[word_num].sym);
		}
			
//		printf("%d\n", word[word_num].sym);
		word_num++;
	}

	return;
}

void returnstatment()
{
	if (word[word_num].sym != RETURNSY)
		Error(224);

	word_num++;
	if (word[word_num].sym == SLPARSY)
	{
		word_num++;
		expression();
		gen_int(RET, 0, 0);
		if (word[word_num].sym != SRPARSY)
			Error(212);

		word_num++;
	}

	if (return_symbol == 0)
	{
		var_level[level] = 0;
		level--;
	}
	gen_int(EXF, 0, 0);
	return;
}

void assignatatment()
{
	int temp_num1 = 0;
	int flag = 0;
	int flag1 = 0;
	int flag2 = 0;

	if (word[word_num].sym != IDSY)
		Error(204);

	word_num++;
	if (word[word_num].sym == MLPARSY)
	{
		flag = Find(word[word_num - 1].name);
		word_num++;
		if (word[word_num].sym == INTNUMSY)
		{
			if (sym_table[flag].value < word[word_num].value)
				Error(227);
		}
		flag1 = sym_table[flag].type;
		expression();

		gen_int(IXX, sym_table[flag].level, sym_table[flag].addr);
		//		gen_int(LOD, 0, -1);

		if (word[word_num].sym != MRPARSY)
			Error(212);
		word_num++;
	}
	else
	{
		flag = Find(word[word_num - 1].name);
		flag1 = sym_table[flag].type;
		if (sym_table[flag].kind == 0)
			Error(225);
		if (sym_table[flag].kind == 1) //����
			gen_int(LDA, sym_table[flag].level, sym_table[flag].addr);
		if (sym_table[flag].kind == 6) //����
			gen_int(LDA, 1, sym_table[flag].addr);
	}

	if (word[word_num].sym != EQSY)
		Error(211);

	word_num++;
	expression();
	flag2 = print_type;
//	printf("%d %d\n", flag1, flag2);
	if ((flag1 == 2) && (flag2 != 0))
		Error(228); //��ֵ���Ͳ�ƥ��

	gen_int(STO, 0, 0);

	return;
}

void returnfuncstatment()
{
	int j = 0;
	int flag = 0;
	int para_num = 0;

	gen_int(CAL, 0, 0);
	if (word[word_num].sym != IDSY)
		Error(204);
	else
	{
		for (j = 0; j < sym_num; j++)
		{
			if (strcmp(sym_table[j].name, word[word_num].name) == 0)
			{
				para_num = sym_table[j].value;
				break;
			}
		}
		//		gen_int(INF, 0, para_num);
		flag = Find(word[word_num].name);
		gen_int(INF, 0, sym_table[flag].value);
	}

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);


	else
	{
		word_num++;
		value_parameter_table();
		if (sym_table[flag].value != para_label_num)
			Error(226);
		if (word[word_num].sym != SRPARSY)
			Error(212);
		word_num++;
	}

	gen_int(JMP, 0, sym_table[flag].addr);
	return;
}

void voidfuncstatment()
{
	int j = 0;
	int flag = 0;
	int para_num = 0;

	gen_int(CAL, 0, 0);
	if (word[word_num].sym != IDSY)
		Error(204);
	else
	{
		for (j = 0; j < sym_num; j++)
		{
			if (strcmp(sym_table[j].name, word[word_num].name))
			{
				para_num = sym_table[j].value;
				break;
			}
		}

		//		gen_int(INF, 0, para_num);
		flag = Find(word[word_num].name);
		gen_int(INF, 0, sym_table[flag].value);
	}

	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);
	else
	{
		word_num++;
		value_parameter_table();
		if (sym_table[flag].value != para_label_num)
			Error(226);
		if (word[word_num].sym != SRPARSY)
			Error(212);
		word_num++;
	}

	gen_int(JMP, 0, sym_table[flag].addr);
	return;
}

void value_parameter_table()
{
	para_label_num = 0;
	if (word[word_num].sym == SRPARSY)
		goto value_parameter_table_end;

	expression();
	gen_int(STO, 0, 1);
	para_label_num++;

	while (1)
	{
		if (word[word_num].sym != COMMASY)
			break;
		else
		{
			word_num++;
			expression();
			gen_int(STO, 0, 1);
			para_label_num++;
		}
	}

value_parameter_table_end:
	return;
}

void main_function()
{
	if (word[word_num].sym != VOIDSY)
		Error(213);

	word_num++;
	if (word[word_num].sym != MAINSY)
		Error(211);

	CreateMainFun();
	strcpy(sym_table[sym_num - 1].name, "main");
	current_level = 1;
	level++;
	word_num++;
	if (word[word_num].sym != SLPARSY)
		Error(211);

	word_num++;
	if (word[word_num].sym != SRPARSY)
		Error(212);

	word_num++;
	if (word[word_num].sym != LLPARSY)
		Error(211);

	word_num++;
	gen_int(INF, 0, -1);
	complex_statment();

	if (word[word_num].sym != LRPARSY)
		Error(212);

	current_level = 0;
	level--;
	word_num++;
	return;
}

//���ű����

void isFull()
{
	if (sym_num >= MAX_SYM)
		Error(301);
}

void NewNode()
{
	sym_table[sym_num].valid = 1; //��Чλ���Ϊ1
	sym_table[sym_num].line = word[word_num].line; //��¼��ǰ�к�
	sym_table[sym_num].level = current_level; //��¼��ǰ��Σ�Ĭ��ȫ��Ϊ0
	strcpy(sym_table[sym_num].name, word[word_num].name); //��¼��ʶ������
}

void CreateConstInt()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 0; //����
	sym_table[sym_num].type = 0; //����
}

void FillConstInt()
{
	sym_table[sym_num].value = word[word_num].value; //��ֵ
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateConstFloat()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 0; //����
	sym_table[sym_num].type = 1; //ʵ��
}

void FillConstFloat()
{
	sym_table[sym_num].value_real = word[word_num].value_real; //��ֵ
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateConstChar()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 0; //����
	sym_table[sym_num].type = 2; //�ַ�
}

void FillConstChar()
{
	sym_table[sym_num].value = word[word_num].value; //��ֵ
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateInt()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 1; //����
	sym_table[sym_num].type = 0; //����
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateIntArray()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 2; //����
	sym_table[sym_num].type = 0; //����
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateFloat()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 1; //����
	sym_table[sym_num].type = 1; //ʵ��
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateFloatArray()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 2; //����
	sym_table[sym_num].type = 1; //ʵ��
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateChar()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 1; //����
	sym_table[sym_num].type = 2; //�ַ�
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateCharArray()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 2; //����
	sym_table[sym_num].type = 2; //�ַ�
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateIntFun()
{
	isFull();
	NewNode();
	para_count = 0;
	used_sym_num = sym_num;
	sym_table[sym_num].kind = 3; //�з��غ���
	sym_table[sym_num].type = 0; //����
	sym_table[sym_num].line = word[word_num].line;
	sym_table[sym_num].addr = pcode_num;
	sym_num++;
}

void CreateFloatFun()
{
	isFull();
	NewNode();
	para_count = 0;
	used_sym_num = sym_num;
	sym_table[sym_num].kind = 3; //�з��غ���
	sym_table[sym_num].type = 1; //ʵ��
	sym_table[sym_num].line = word[word_num].line;
	sym_table[sym_num].addr = pcode_num;
	sym_num++;
}

void CreateCharFun()
{
	isFull();
	NewNode();
	para_count = 0;
	used_sym_num = sym_num;
	sym_table[sym_num].kind = 3; //�з��غ���
	sym_table[sym_num].type = 2; //�ַ�
	sym_table[sym_num].line = word[word_num].line;
	sym_table[sym_num].addr = pcode_num;
	sym_num++;
}

void CreateVoidFun()
{
	isFull();
	NewNode();
	para_count = 0;
	used_sym_num = sym_num;
	sym_table[sym_num].kind = 4; //�޷��غ���
								 //	sym_table[sym_num].type = 0; 
	sym_table[sym_num].line = word[word_num].line;
	sym_table[sym_num].addr = pcode_num;
	sym_num++;
}

void CreateMainFun()
{
	isFull();
	NewNode();
	para_count = 0;
	used_sym_num = sym_num;
	sym_table[sym_num].kind = 5; //�޷��غ���
								 //	sym_table[sym_num].type = 0; 
	sym_table[sym_num].line = word[word_num].line;
	sym_num++;
}

void CreateIntPara()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 6; //����
	sym_table[sym_num].type = 0; //����
	sym_table[sym_num].line = word[word_num].line;
	para_count++;
	sym_num++;
}

void CreateFloatPara()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 6; //����
	sym_table[sym_num].type = 1; //ʵ��
	sym_table[sym_num].line = word[word_num].line;
	para_count++;
	sym_num++;
}

void CreateCharPara()
{
	isFull();
	NewNode();
	sym_table[sym_num].kind = 6; //����
	sym_table[sym_num].type = 2; //�ַ�
	sym_table[sym_num].line = word[word_num].line;
	para_count++;
	sym_num++;
}

int isReDefine()
{
	int flag = 0;
	int j = 0;

	for (j = sym_num; j >= 0; j--)
	{
		if (sym_table[j].valid == 0)
			continue;

/*		if ((sym_table[j].level == 1) && (sym_table[j].level != current_level))
			break;
			
		if ((sym_table[j].level == 0) && (sym_table[j].level != current_level))
			continue;*/

		if ((current_level == 1) && (sym_table[j].level == 0))
			break;

		if ((current_level == 0) && (sym_table[j].level == 1))
			continue;

		if (strcmp(sym_table[j].name, word[word_num].name) == 0)
		{
			flag = 1;
			break;
		}
	}

//	flag = 0;
	return flag;
}

int Find(char name[])
{
	int flag = 0;
	int j = 0;

	if (current_level == 0)
	{
		for (j = sym_num; j >= 0; j--)
		{
			if (sym_table[j].valid == 0)
				continue;
			if (sym_table[j].level == 1)
				continue;
			if (strcmp(sym_table[j].name, name) == 0)
			{
				flag = 1;
				break;
			}
		}
	}

	if (current_level == 1)
	{
		for (j = sym_num; j >= 0; j--)
		{
			if (sym_table[j].valid == 0)
				continue;
			if (sym_table[j].level != current_level)
				break;
			if (strcmp(sym_table[j].name, name) == 0)
			{
				flag = 1;
				break;
			}
		}

		if (flag == 0)
		{
			for (j = sym_num; j >= 0; j--)
			{
				if (sym_table[j].valid == 0)
					continue;
				if (sym_table[j].level == 1)
					continue;
				if (strcmp(sym_table[j].name, name) == 0)
				{
					flag = 1;
					break;
				}
			}
		}
	}

	if (flag == 1)
		return j;
	else
		return -1;
	return flag;
}

//��������

void gen_int(int op, int lev, int x)
{
	pcode[pcode_num].op = op;
	pcode[pcode_num].lev = lev;
	pcode[pcode_num].x = x;
	pcode_num++;
}

void gen_float(int op, int lev, float y)
{
	pcode[pcode_num].op = op;
	pcode[pcode_num].lev = lev;
	pcode[pcode_num].y = y;
	pcode_num++;
//	printf("%d %d %lf\n", op, lev, y);
}

void gen_char(int op, int lev, char z)
{
	pcode[pcode_num].op = op;
	pcode[pcode_num].lev = lev;
	pcode[pcode_num].z = z;
	pcode_num++;
}

void pcode_print()
{
	fp2 = fopen("pcode.txt", "w");
	int j = 0;
	if (pcode[pcode_num - 1].op == EXF)
		pcode[pcode_num - 1].x = -1;
	else
	{
		pcode[pcode_num].op = EXF;
		pcode[pcode_num].lev = 0;
		pcode[pcode_num].x = -1;
		pcode_num++;
	}
		

	for (j = 1; j < pcode_num; j++)
	{
//		printf("%d %d %d %lf\n", pcode[j].op, pcode[j].lev, pcode[j].y, pcode[j].y);
		if (pcode[j].x != 0)
			printf("%d %s %d %d %d\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].x);
		else if (pcode[j].y != 0)
			printf("%d %s %d %d %lf\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].y);
		else if (pcode[j].z != 0)
			printf("%d %s %d %d %d\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].z);
		else
			printf("%d %s %d %d %d\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].x);
	}

	for (j = 1; j < pcode_num; j++)
	{
//		printf("%d %d %d %lf\n", pcode[j].op, pcode[j].lev, pcode[j].y, pcode[j].y);
		if (pcode[j].x != 0)
			fprintf(fp2, "%d %s %d %d %d\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].x);
		else if (pcode[j].y != 0)
			fprintf(fp2, "%d %s %d %d %lf\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].y);
		else if (pcode[j].z != 0)
			fprintf(fp2, "%d %s %d %d %d\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].z);
		else
			fprintf(fp2, "%d %s %d %d %d\n", j, pcode_op[pcode[j].op], pcode[j].op, pcode[j].lev, pcode[j].x);
		fflush(fp2);
	}

	return;
}

//����ִ��

void Interpret()
{
	float run_stack[MAX_RUN];
	float compute_stack[MAX_RUN];
	int type_stack[MAX_RUN];
	int num_stack[MAX_STACK_NUM];
	int compute_num_stack[MAX_STACK_NUM];
	int run_num = 0;
	int num_num = 0;
	int type_num = 0;
	int pcode_line = 0;
	int enter_num = 0;
	int compute_num = 0;
	int compute_num_num = 0;
	int in_char = 0;
	int return_stack[100];
	int return_num = 0;
	int inf_num = 0;
	int jump_num = 0;
	int read_num = 0;
	int start = 0;
	float run_a = 0;
	float run_b = 0;
	int i = 0;
	int k = 0;
	char ch[50];
	char pcode_addr[32];
	char temp_char[32];

	printf("Please Enter The PCode Address \n");
	scanf("%s", pcode_addr);

	fp3 = fopen(pcode_addr, "r");
	while ((ch[0] = fgetc(fp3)) != EOF) //�ж�Դ��������
	{
		if (ch[0] == '\n')
			enter_num++;
	}
	fclose(fp3);

	fp3 = fopen(pcode_addr, "r");
	pcode_line = 1;
	while (!feof(fp3)) //����Դ����
	{
		int j = 0;
		int flag = 0;

		fscanf(fp3, "%d %s %d %d %s\n", &i, &ch, &pcode_inter[pcode_line].op, &pcode_inter[pcode_line].lev, &temp_char);

		for (j = 0; j < 32; j++)
		{
			if (temp_char[j] == '.')
			{
				flag = j;
				break;
			}

			else
				flag = 0;
		}

		if (flag != 0)
		{
			char temp1[32];
			char temp2[32];
			float temp_num1 = 0;
			float temp_num2 = 0;
			int k = 0;
			for (j = 0, k = 0; j < flag; j++, k++)
			{
				if (temp_char[j] == '-')
				{
					k--;
					continue;
				}
				temp1[k] = temp_char[j];
			}
			temp1[flag] = '\0';
			for (j = flag + 1, i = 0; j < 32; j++, i++)
			{
				if (isdigit(temp_char[j]))
					temp2[i] = temp_char[j];
				else
					break;
			}
			temp2[i] = '\0';

			temp_num1 = atoi(temp1);
			temp_num2 = atoi(temp2);

			for (j = 0; j < i + 1; j++)
				temp_num2 = temp_num2 / 10;

			pcode_inter[pcode_line].num = temp_num1 + temp_num2;
		}

		else
		{
			int temp_num = 0;
			temp_num = atoi(temp_char);
			pcode_inter[pcode_line].num = temp_num;
			//			pcode_inter[pcode_line].float_valid = 0;
		}

		pcode_line++;
		if (pcode_line == enter_num + 1)
			break;
	}
	fclose(fp3);

	//	for (i = 1; i < enter_num + 1; i++)
	//		printf("%d %d %d %lf\n", i, pcode_inter[i].op, pcode_inter[i].lev, pcode_inter[i].num);
	//	getch();

	num_stack[0] = 0; //��ʼ��
	run_stack[0] = 0; //Ԥ��
	run_stack[1] = 0;
	num_num++; //��ʼ��
	run_num++;
	run_num++;

	for (i = 1; i < enter_num + 1; i++) //ȷ��main�������
	{
		if (pcode_inter[i].op == INF)
		{
			if (pcode_inter[i].num == -1)
			{
				start = i;
				break;
			}
		}
	}

	run_num = run_num + pcode_inter[start - 1].num; //������ջ��Ϊȫ�ֱ�������ռ�
	num_stack[num_num] = run_num; //��¼ָ��λ��
	num_num++;
	run_num++;
	run_stack[run_num++] = 0; //Ԥ��
	run_stack[run_num++] = 0;

	for (i = start; i < enter_num + 1; i++)
	{
//		printf("%d\n", i);
		switch (pcode_inter[i].op)
		{
		case LDI:
			compute_stack[compute_num++] = pcode_inter[i].num;
//			printf("LDI %d %d %d\n", i, compute_num - 1, (int)compute_stack[compute_num - 1]);
			break;

		case LDA:
			if (pcode_inter[i].lev == 0)
				compute_stack[compute_num++] = pcode_inter[i].num + 2;
			else
				compute_stack[compute_num++] = num_stack[num_num - 1] + pcode_inter[i].num + 2;
//			printf("LDA %d %d %d\n", i, num_stack[num_num - 1], (int)compute_stack[compute_num - 1]);
			break;

		case LOD:
			if (pcode_inter[i].num == -1)
			{
//				printf("LOD %d %d %d\n", i, compute_num - 1, (int)compute_stack[compute_num - 1]);
				compute_stack[compute_num - 1] = run_stack[(int)compute_stack[compute_num - 1]];
				
			}

			else
			{
				if (pcode_inter[i].lev == 0)
					compute_stack[compute_num++] = run_stack[(int)pcode_inter[i].num + 2];
				else
				{
//					printf("%d\n", inf_num);
					if (inf_num > 0)
					{
						compute_stack[compute_num++] = run_stack[num_stack[num_num - 2] + (int)pcode_inter[i].num + 2];
//						printf("LOD %d %d %d\n", i, num_stack[num_num - 2] + (int)pcode_inter[i].num + 2, (int)run_stack[num_stack[num_num - 2] + (int)pcode_inter[i].num + 2]);					
					}
						
					else
						compute_stack[compute_num++] = run_stack[num_stack[num_num - 1] + (int)pcode_inter[i].num + 2];
				}
			}
//			printf("LOD %d %d %d\n", i, (int)compute_stack[compute_num - 1], (int)compute_stack[compute_num - 1]);
			break;

		case IXX:
			if (pcode_inter[i].lev == 0)
				compute_stack[compute_num - 1] = (int)compute_stack[compute_num - 1] + (int)pcode_inter[i].num + 2;
			else
				compute_stack[compute_num - 1] = (int)compute_stack[compute_num - 1] + num_stack[num_num - 1] + (int)pcode_inter[i].num + 2;
//			printf("IXX %d %d %d\n", i, compute_num - 1, (int)compute_stack[compute_num - 1]);
			break;

		case STO:
			if (read_num == 1)
			{
				read_num = 0;
				//				printf("STO %d %d %d\n", i, (int)compute_stack[compute_num - 1], (int)compute_stack[compute_num - 2]);
				run_stack[(int)compute_stack[compute_num - 1]] = compute_stack[compute_num - 2];
				compute_num = compute_num - 2;

				break;
			}

			if (pcode_inter[i].num == 1)
			{
				if (inf_num >= 0)
				{
					inf_num--;
					run_stack[run_num++] = compute_stack[compute_num - 1];
					compute_num--;
					//					printf("STO %d %d %d\n", i, run_num - 1, (int)run_stack[run_num - 1]);
				}
				break;
			}
     		
			run_stack[(int)compute_stack[compute_num - 2]] = compute_stack[compute_num - 1];
//			printf("STO %d %d %d\n", i, (int)compute_stack[compute_num - 2], (int)run_stack[(int)compute_stack[compute_num - 2]]);
//			printf("run_num %d %d\n", i, run_num);
			compute_num = compute_num - 2;
			break;

		case JPC:
			if (compute_stack[compute_num - 1] == 0)
				i = (int)pcode_inter[i].num - 1;
			compute_num--;
//			printf("JPC %d %d\n", i, (int)compute_stack[compute_num - 1]);
			break;

		case JMP:

			if (jump_num == 1)
			{
				return_stack[return_num++] = i;
				jump_num = 0;
			}
			i = (int)pcode_inter[i].num - 1;
			break;

		case RED:
			scanf("%s", &ch);
			if ((isdigit(ch[0]) != 0) || (ch[0] == '-'))
				compute_stack[compute_num++] = atoi(ch);
			else
				compute_stack[compute_num++] = (int)ch[0];
			read_num = 1;
//			printf("RED %d", run_num);
			//			printf("RED %d %d %d\n", i, read_num, (int)compute_stack[compute_num - 1]);
			break;

		case WRR:
			if (pcode_inter[i].lev == 0) //�ַ�
			{
				if (pcode_inter[i].num == -1)
					printf("\n");
				if (pcode_inter[i].num == 0)
				{
					printf("%c", (int)compute_stack[compute_num - 1]);
					compute_num--;
				}

				printf("%c", (int)pcode_inter[i].num);
			}
			else if (pcode_inter[i].lev == 1) //����
				printf("%d", (int)compute_stack[compute_num - 1]);
			else if (pcode_inter[i].lev == 2) //����
				printf("%lf", compute_stack[compute_num - 1]);
			//			compute_num--;
			break;

			//		case INT:
			//			break;

		case ADD:
			compute_stack[compute_num - 2] = compute_stack[compute_num - 1] + compute_stack[compute_num - 2];
			compute_num--;
			break;

		case SUB:
			//			printf("SUB %d %d %d\n", i, (int)compute_stack[compute_num - 1], (int)compute_stack[compute_num - 2]);
			compute_stack[compute_num - 2] = compute_stack[compute_num - 2] - compute_stack[compute_num - 1];
			compute_num--;
			break;

		case MUL:
			compute_stack[compute_num - 2] = compute_stack[compute_num - 1] * compute_stack[compute_num - 2];
			compute_num--;
			break;

		case DIV:
			compute_stack[compute_num - 2] = compute_stack[compute_num - 2] / compute_stack[compute_num - 1];
			compute_num--;
			break;

		case MUS:
			compute_stack[compute_num - 1] = compute_stack[compute_num - 1] * -1;
			break;

		case GRT:
			if (compute_stack[compute_num - 2] > compute_stack[compute_num - 1])
				compute_stack[compute_num - 2] = 1;
			else
				compute_stack[compute_num - 2] = 0;
			compute_num--;
			break;

		case LES:
			if (compute_stack[compute_num - 2] < compute_stack[compute_num - 1])
				compute_stack[compute_num - 2] = 1;
			else
				compute_stack[compute_num - 2] = 0;
			compute_num--;
			break;

		case EQL:
			if (compute_stack[compute_num - 2] == compute_stack[compute_num - 1])
				compute_stack[compute_num - 2] = 1;
			else
				compute_stack[compute_num - 2] = 0;
			compute_num--;
			break;

		case NEQ:
			if (compute_stack[compute_num - 2] != compute_stack[compute_num - 1])
				compute_stack[compute_num - 2] = 1;
			else
				compute_stack[compute_num - 2] = 0;
			compute_num--;
			break;

		case GEQ:
			if (compute_stack[compute_num - 2] >= compute_stack[compute_num - 1])
				compute_stack[compute_num - 2] = 1;
			else
				compute_stack[compute_num - 2] = 0;
			compute_num--;
			break;

		case LER:
			if (compute_stack[compute_num - 2] <= compute_stack[compute_num - 1])
				compute_stack[compute_num - 2] = 1;
			else
				compute_stack[compute_num - 2] = 0;
			compute_num--;
			break;

		case CAL:
			num_stack[num_num] = run_num; //��¼ָ��λ��
			compute_num_stack[compute_num_num] = compute_num;
			num_num++;
			compute_num_num++;
			run_stack[run_num++] = 0; //Ԥ��
			run_stack[run_num++] = 0;
//			printf("%d %d\n", i, run_num);
			jump_num = 1;
			break;

		case RET:
			compute_stack[compute_num_stack[compute_num_num - 1]] = compute_stack[compute_num - 1];
//			printf("RET %d %d %d\n", i, compute_num_stack[compute_num_num - 1], (int)compute_stack[compute_num - 1]);
			break;

		case EXF:
			if (pcode_inter[i].num == -1)
			{
//				printf("EXF %d %d\n", i, (int)pcode_inter[i].num);
				goto pcode_inter_end;
			}
				
			i = return_stack[--return_num];
			run_num = num_stack[--num_num];
			compute_num = compute_num_stack[--compute_num_num] + 1;
//			printf("EXF %d %d %d %d\n", i, return_num, run_num, compute_num);
			break;

		case INF:
//			printf("INF %d %d\n", i, run_num);
			if (pcode_inter[i].lev != 0)
			{
				run_num = run_num + pcode_inter[i].lev;
				break;
			}
				
			inf_num = (int)pcode_inter[i].num;
			break;
		}

//				for (k = 0; k < compute_num; k++)
//					printf("%d ", (int)compute_stack[k]);
//				printf("\n");
	}

pcode_inter_end:
	return;

}

// ������

void Error(int e)
{
	switch (e)
	{

		//�ʷ�����
	case 101: printf("line:%d,TokenError 101,̾�ŵ�������!\n", line + 1); break;
	case 102: printf("line:%d,TokenError 102,ȱ�ٺ�����!\n", line + 1); break;
	case 103: printf("line:%d,TokenError 103,�����ڷǺϷ��ַ�!\n", line + 1); break;
	case 104: printf("line:%d,TokenError 104,�������ڽ��޵��ַ�!\n", line + 1); break;
	case 105: printf("line:%d,TokenError 105,���ֲ�����0��ͷ\n", line + 1); break;

		//�﷨����
	case 201: printf("line:%d,GrammarError 201,ȱ��������!\n",word[word_num].line + 1); break;
	case 202: printf("line:%d,GrammarError 202,Ӧ��const��ͷ!\n", word[word_num].line + 1); break;
	case 203: printf("line:%d,GrammarError 203,Ӧ�ԣ�����!\n",word[word_num].line + 1); break;
	case 204: printf("line:%d,GrammarError 204,ӦΪ��ʶ��!\n", word[word_num].line + 1); break;
	case 205: printf("line:%d,GrammarError 205,ӦΪ=!\n", word[word_num].line + 1); break;
	case 206: printf("line:%d,GrammarError 206,������!\n", word[word_num].line + 1); break;
	case 207: printf("line:%d,GrammarError 207,���޷�������!\n", word[word_num].line + 1); break;
	case 208: printf("line:%d,GrammarError 208,��ʵ��!\n", word[word_num].line + 1); break;
	case 209: printf("line:%d,GrammarError 209,���ַ�!\n", word[word_num].line + 1); break;
	case 210: printf("line:%d,GrammarError 210,�����ͱ�ʶ��!\n", word[word_num].line + 1); break;
	case 211: printf("line:%d,GrammarError 211,��ʽ���Ϸ�!\n", word[word_num].line + 1); break;
	case 212: printf("line:%d,GrammarError 212,ȱ�ٺ�����!\n",word[word_num].line + 1); break;
	case 213: printf("line:%d,GrammarError 213,Ӧ��void��ͷ!\n", word[word_num].line + 1); break;
	case 214: printf("line:%d,GrammarError 214,�����!\n", word[word_num].line + 1); break;
	case 215: printf("line:%d,GrammarError 215,Ӧ��if��ͷ!\n", word[word_num].line + 1); break;
	case 216: printf("line:%d,GrammarError 216,Ӧ��while��ͷ!\n", word[word_num].line + 1); break;
	case 217: printf("line:%d,GrammarError 217,Ӧ��switch��ͷ!\n", word[word_num].line + 1); break;
	case 218: printf("line:%d,GrammarError 218,Ӧ��case��ͷ!\n", word[word_num].line + 1); break;
	case 219: printf("line:%d,GrammarError 219,�ǿ�ö�ٳ���!\n", word[word_num].line + 1); break;
	case 220: printf("line:%d,GrammarError 220,ӦΪ:!\n", word[word_num].line + 1); break;
	case 221: printf("line:%d,GrammarError 221,Ӧ��default��ͷ!\n", word[word_num].line + 1); break;
	case 222: printf("line:%d,GrammarError 222,Ӧ��scanf��ͷ!\n", word[word_num].line + 1); break;
	case 223: printf("line:%d,GrammarError 223,Ӧ��printf��ͷ!\n", word[word_num].line + 1); break;
	case 224: printf("line:%d,GrammarError 224,Ӧ��return��ͷ!\n", word[word_num].line + 1); break;
	case 225: printf("line:%d,GrammarError 225,�������ø�ֵ\n", word[word_num].line + 1); break;
	case 226: printf("line:%d,GrammarError 226,ʵ�θ�����ƥ��\n", word[word_num].line + 1); break;
	case 227: printf("line:%d,GrammarError 227,����Խ��\n", word[word_num].line + 1); break;
	case 228: printf("line:%d,GrammarError 228,��ֵ���Ͳ�ƥ��\n", word[word_num].line + 1); break;

		//���ű����
	case 301: printf("ID:%s,line:%d,SymbolTableError 301,���ű�����!\n", word[word_num].name, word[word_num].line + 1); break;
	case 302: printf("ID:%s,line:%d,SymbolTableError 302,��ʶ���ض���!\n", word[word_num].name, word[word_num].line + 1); break;

		//��������
	case 401: printf("line:%d,PcodeProductError 401,��ʶ��δ����!\n", word[word_num].line + 1); break;
	case 402: printf("line:%d,PcodeProductError 402,������������!\n", word[word_num].line + 1); break;
		
	default: printf("Unknown Error!\n"); break;

	}

	error_flag = 1;
}
