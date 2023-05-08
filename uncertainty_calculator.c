#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#define MAX_SIZE 1000

typedef struct {    //����һ�������ջ
    char Operator[MAX_SIZE];
    int top;
} OptorStack;

typedef struct {    //����һ��������ջ
    double Operand[MAX_SIZE];
    int top;
} OpndStack;

typedef struct variable_quantity{   //����һ�����ݼ�
    char t;         //����
    double ave;     //ƽ��ֵ
    double re_ave;  //������ƽ��ֵ
    double a_u;     //A�಻ȷ����
    double n_u;       //�ϳɲ�ȷ����
    double dt;      //ƫ����
} VQ;

void input(double a[], int n);
double aver(const double a[],int n);
double A_uncertainty(const double a[],double ave,int n);
double N_uncertainty(double A,double B);
void once_uncertainty_calculate(VQ x[],int i,int N);      //һ�β�ȷ���ȼ���
double indirect_uncertainty(VQ x[],int n);

void InitOptor(OptorStack *S);
void InitOpnd(OpndStack *S);
int PopOptor(OptorStack *S);
int PopOpnd(OpndStack *S);
int PushOptor(OptorStack *S,char ch);
int PushOpnd(OpndStack *S,double ch);
char GetOptor(OptorStack *S);
double GetOpnd(OpndStack *S);
double Calculate(double a,double b,char op);
int change(char ch);
int Compare(char ch1,char ch2);
int Check(char *S,int len);
double calculator(VQ X[],int n);        //������ʽ��ֵ

int main()
{
    int sum,N;
    double i_m,i_u;
    printf("���������������");
    scanf("%d",&sum);
    printf("����������������");
    scanf("%d",&N);
    VQ xx[9999];
    for (int i = 0; i <sum; ++i) {
        once_uncertainty_calculate(xx,i,N);
    }
    printf("�������Ӳ���ֵ�ı��ʽ��\n");
    i_m= calculator(xx,sum);
    for (int i = 0; i <sum; ++i) {
        printf("�������%d������ƫ�����ı��ʽ��\n",i+1);
        xx[i].dt=calculator(xx,sum);
    }
    i_u= indirect_uncertainty(xx,sum);
    for (int i = 0; i <sum; ++i) {
        printf("��������%c ƽ������%lf ������ƽ������%lf \nA�಻ȷ���ȣ�%lf �ϳɲ�ȷ���ȣ�%lf\n\n"
               ,xx[i].t,xx[i].ave,xx[i].re_ave,xx[i].a_u,xx[i].n_u);
    }
    printf("��Ӳ���ֵΪ��%lf\n",i_m);
    printf("��Ӳ�ȷ����Ϊ��%lf\n\n",i_u);
    printf("Created by Martin Y\n\n");
    system("pause");
    return 0;
}
void once_uncertainty_calculate(VQ x[],int i,int N)
{
    char s;
    double d[9999];
    double b_c,F;
    printf("�������%d����������:\n",i+1);
    scanf("%s",&s);
    printf("������ñ�����Ӧ��B�಻ȷ����:\n");
    scanf("%lf",&b_c);
    printf("�����������:\n");
    scanf("%lf",&F);
    printf("������ñ�����Ӧ�Ĳ�������:\n");
    input(d,N);
    x[i].t=s;
    x[i].ave=aver(d,N);
    x[i].re_ave=x[i].ave-F;
    x[i].a_u=A_uncertainty(d,x[i].re_ave,N);
    x[i].n_u=N_uncertainty(x[i].a_u,b_c);
    printf("\n");
}
double aver(const double a[],int n)
{
    double sum=0;
    for(int i=0;i<n;i++)
        sum+=a[i];
    return sum/n;
}
double A_uncertainty(const double a[],double ave,int n)
{
    double sum=0;
    for(int i=0;i<n;i++)
        sum+=pow(a[i]-ave,2);
    return sqrt(sum/(n-1));
}
double N_uncertainty(double A,double B)
{
    return sqrt(pow(A,2)+pow(B,2));
}
void input(double a[], int n)
{
    for(int i=0;i<n;i++)
        scanf("%lf",&a[i]);
}
double indirect_uncertainty(VQ x[],int n)
{
    double sum=0;
    for (int i = 0; i < n; ++i) {
        sum+= pow(x[i].dt * x[i].n_u,2);
    }
    return sqrt(sum);
}

double calculator(VQ X[],int n)
{
    char expre[9999],opdstr[9999];
    double opd;  //expre�����洢���ʽ��opd�����������
    int len;
    OptorStack Optor;
    OpndStack Opnd;
    InitOptor(&Optor);
    InitOpnd(&Opnd);
    PushOptor(&Optor,'#');
    scanf("%s",expre);
    len = strlen(expre);
    if (Check(expre,len) == 0) {
        printf("����ı��ʽ���Ϸ������ܴ��ڶ����ַ�\n");
        exit(19);
    }
    int j=0,k=0,l=0;
    double x,y;
    expre[len] = '#';   //��expre�����Ӹ�#��β��־
    for (int i=0; i<=len; i++) {
        if ((expre[i]>='a' && expre[i]<='z') || (expre[i]>='A' && expre[i]<='Z')) {
            for(int g=0;g<n;g++){
                if(expre[i]==X[g].t)
                    opd = X[g].re_ave;//�������ַ��������������opd��
            }
            l = 1;
            continue;
        }
        else if ((expre[i]>='0' && expre[i]<='9') || expre[i] == '.') {
            opdstr[k++] = expre[i];   //�������ַ��������������opd��
            j = 1;
            continue;
        }
        if(l) {
            PushOpnd(&Opnd,opd);
            l=0;
        }
        if(j) {
            opdstr[k] = '\0';
            PushOpnd(&Opnd,atof(opdstr));
            j=0;
            k=0;
        }
        switch (Compare(GetOptor(&Optor),expre[i])) {  //�Ƚ������ջ��ջ��������������expre[i]�����ȼ�
            case '<':
                PushOptor(&Optor,expre[i]);
                break;
            case '=':
                PopOptor(&Optor);
                break;
            case '>':
                y = GetOpnd(&Opnd),PopOpnd(&Opnd);
                x = GetOpnd(&Opnd),PopOpnd(&Opnd);
                PushOpnd(&Opnd,Calculate(x,y,GetOptor(&Optor)));
                PopOptor(&Optor);
                i--;
                break;
        }
    }
    double final_result = GetOpnd(&Opnd);
    return final_result;
}
/*��ʼ�������ջ*/
void InitOptor(OptorStack *S) {
    S->top = -1;
}
/*��ʼ��������ջ*/
void InitOpnd(OpndStack *S) {
    S->top = -1;
}
/*�������ջ*/
int PopOptor(OptorStack *S) {
    if(S->top==-1) {
        printf("�����ջΪ��\n");
        exit(10);
    }
    S->top--;
    return 1;
}
/*��������ջ*/
int PopOpnd(OpndStack *S) {
    if(S->top==-1) {
        printf("������ջΪ��\n");
        exit(11);
    }
    S->top--;
    return 1;
}

/*�������ջ*/
int PushOptor(OptorStack *S,char ch) {
    if(S->top==MAX_SIZE-1) {
        printf("�����ջ��\n");
        exit(12);
    }
    S->top++;
    S->Operator[S->top]=ch;
    return 1;
}
/*��������ջ*/
int PushOpnd(OpndStack *S,double ch) {
    if(S->top==MAX_SIZE-1) {
        printf("������ջ��\n");
        exit(13);
    }
    S->top++;
    S->Operand[S->top]=ch;
    return 1;
}
/*ȡ�����ջջ��Ԫ��*/
char GetOptor(OptorStack *S) {
    if(S->top==-1) {
        printf("�����ջΪ��\n");
        exit(17);
    }
    return S->Operator[S->top];
}
/*ȡ������ջջ��Ԫ��*/
double GetOpnd(OpndStack *S) {
    if(S->top==-1) {
        printf("������ջΪ��\n");
        exit(18);
    }
    return S->Operand[S->top];
}
/*���ʽ���㺯��*/
double Calculate(double a,double b,char op) {
    double result;
    if(op=='+') result=a+b;
    if(op=='-') result=a-b;
    if(op=='*') result=a*b;
    if(op=='/') {   //Ҫ��ֹ����Ϊ0
        if(b==0) {
            printf("ERROR:��������Ϊ0\n");
            exit(15);
        }
        result=a/b;
    }
    return result;    //���ؽ��result
}

int change(char ch) {
    switch(ch) {
        case '+':
            return 0;
        case '-':
            return 1;
        case '*':
            return 2;
        case '/':
            return 3;
        case '(':
            return 4;
        case ')':
            return 5;
        case '#':
            return 6;
    }
}
/*����һ��������ȹ�ϵ����*/
char cmp[7][8]= {">><<<>>",">><<<>>",">>>><>>",">>>><>>","<<<<<=?",">>>>?>>","<<<<<?="};
/*����ȽϺ������������ȼ�:>��<��=*/
int Compare(char ch1,char ch2) {
    if(cmp[change(ch1)][change(ch2)]=='?') {
        printf("������ʽ����");
        exit(16);
    }
    return cmp[change(ch1)][change(ch2)];
}
/*��麯��,�����������ı��ʽ�Ĳ�������������ĺϷ���*/
int Check(char *S,int len) {
    int i;
    for(i=0; i<len; i++) {
        if(S[i]>='0'&&S[i]<='9')continue;
        if((S[i]>='A'&&S[i]<='Z') || (S[i]>='a'&&S[i]<='z'))continue;
        if(S[i]=='('||S[i]==')'||S[i]=='*'||S[i]=='/'||S[i]=='+'||S[i]=='-'||S[i]=='.')continue;
        return 0;
    }
    return 1;
}

//
// Created by Martin Yang on 2022/9/29.
//
