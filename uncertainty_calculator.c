#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#define MAX_SIZE 1000

typedef struct {    //定义一个运算符栈
    char Operator[MAX_SIZE];
    int top;
} OptorStack;

typedef struct {    //定义一个操作数栈
    double Operand[MAX_SIZE];
    int top;
} OpndStack;

typedef struct variable_quantity{   //定义一个数据集
    char t;         //变量
    double ave;     //平均值
    double re_ave;  //修正后平均值
    double a_u;     //A类不确定度
    double n_u;       //合成不确定度
    double dt;      //偏导数
} VQ;

void input(double a[], int n);
double aver(const double a[],int n);
double A_uncertainty(const double a[],double ave,int n);
double N_uncertainty(double A,double B);
void once_uncertainty_calculate(VQ x[],int i,int N);      //一次不确定度计算
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
double calculator(VQ X[],int n);        //键入表达式求值

int main()
{
    int sum,N;
    double i_m,i_u;
    printf("请输入变量总数：");
    scanf("%d",&sum);
    printf("请输入样本总数：");
    scanf("%d",&N);
    VQ xx[9999];
    for (int i = 0; i <sum; ++i) {
        once_uncertainty_calculate(xx,i,N);
    }
    printf("请输入间接测量值的表达式：\n");
    i_m= calculator(xx,sum);
    for (int i = 0; i <sum; ++i) {
        printf("请输入第%d个变量偏导数的表达式：\n",i+1);
        xx[i].dt=calculator(xx,sum);
    }
    i_u= indirect_uncertainty(xx,sum);
    for (int i = 0; i <sum; ++i) {
        printf("变量名：%c 平均数：%lf 修正后平均数：%lf \nA类不确定度：%lf 合成不确定度：%lf\n\n"
               ,xx[i].t,xx[i].ave,xx[i].re_ave,xx[i].a_u,xx[i].n_u);
    }
    printf("间接测量值为：%lf\n",i_m);
    printf("间接不确定度为：%lf\n\n",i_u);
    printf("Created by Martin Y\n\n");
    system("pause");
    return 0;
}
void once_uncertainty_calculate(VQ x[],int i,int N)
{
    char s;
    double d[9999];
    double b_c,F;
    printf("请输入第%d个变量名称:\n",i+1);
    scanf("%s",&s);
    printf("请输入该变量对应的B类不确定度:\n");
    scanf("%lf",&b_c);
    printf("请输入初读数:\n");
    scanf("%lf",&F);
    printf("请输入该变量对应的测量数据:\n");
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
    double opd;  //expre用来存储表达式，opd用来存操作数
    int len;
    OptorStack Optor;
    OpndStack Opnd;
    InitOptor(&Optor);
    InitOpnd(&Opnd);
    PushOptor(&Optor,'#');
    scanf("%s",expre);
    len = strlen(expre);
    if (Check(expre,len) == 0) {
        printf("输入的表达式不合法，可能存在多余字符\n");
        exit(19);
    }
    int j=0,k=0,l=0;
    double x,y;
    expre[len] = '#';   //在expre数组后加个#结尾标志
    for (int i=0; i<=len; i++) {
        if ((expre[i]>='a' && expre[i]<='z') || (expre[i]>='A' && expre[i]<='Z')) {
            for(int g=0;g<n;g++){
                if(expre[i]==X[g].t)
                    opd = X[g].re_ave;//将数字字符存入操作数数组opd中
            }
            l = 1;
            continue;
        }
        else if ((expre[i]>='0' && expre[i]<='9') || expre[i] == '.') {
            opdstr[k++] = expre[i];   //将数字字符存入操作数数组opd中
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
        switch (Compare(GetOptor(&Optor),expre[i])) {  //比较运算符栈的栈顶运算符和运算符expre[i]的优先级
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
/*初始化运算符栈*/
void InitOptor(OptorStack *S) {
    S->top = -1;
}
/*初始化操作数栈*/
void InitOpnd(OpndStack *S) {
    S->top = -1;
}
/*运算符出栈*/
int PopOptor(OptorStack *S) {
    if(S->top==-1) {
        printf("运算符栈为空\n");
        exit(10);
    }
    S->top--;
    return 1;
}
/*操作数出栈*/
int PopOpnd(OpndStack *S) {
    if(S->top==-1) {
        printf("操作数栈为空\n");
        exit(11);
    }
    S->top--;
    return 1;
}

/*运算符入栈*/
int PushOptor(OptorStack *S,char ch) {
    if(S->top==MAX_SIZE-1) {
        printf("运算符栈满\n");
        exit(12);
    }
    S->top++;
    S->Operator[S->top]=ch;
    return 1;
}
/*操作数入栈*/
int PushOpnd(OpndStack *S,double ch) {
    if(S->top==MAX_SIZE-1) {
        printf("操作数栈满\n");
        exit(13);
    }
    S->top++;
    S->Operand[S->top]=ch;
    return 1;
}
/*取运算符栈栈顶元素*/
char GetOptor(OptorStack *S) {
    if(S->top==-1) {
        printf("运算符栈为空\n");
        exit(17);
    }
    return S->Operator[S->top];
}
/*取操作数栈栈顶元素*/
double GetOpnd(OpndStack *S) {
    if(S->top==-1) {
        printf("操作数栈为空\n");
        exit(18);
    }
    return S->Operand[S->top];
}
/*表达式运算函数*/
double Calculate(double a,double b,char op) {
    double result;
    if(op=='+') result=a+b;
    if(op=='-') result=a-b;
    if(op=='*') result=a*b;
    if(op=='/') {   //要防止除数为0
        if(b==0) {
            printf("ERROR:除数不能为0\n");
            exit(15);
        }
        result=a/b;
    }
    return result;    //返回结果result
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
/*定义一个算符优先关系矩阵*/
char cmp[7][8]= {">><<<>>",">><<<>>",">>>><>>",">>>><>>","<<<<<=?",">>>>?>>","<<<<<?="};
/*算符比较函数，返回优先级:>或<或=*/
int Compare(char ch1,char ch2) {
    if(cmp[change(ch1)][change(ch2)]=='?') {
        printf("输入表达式错误");
        exit(16);
    }
    return cmp[change(ch1)][change(ch2)];
}
/*检查函数,用来检查输入的表达式的操作数和运算符的合法性*/
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
