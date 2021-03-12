/***************************************************
 * File: util.cpp
 * Function: 实现 Mini C 编译器的解析部分和词法分析部分时，
 *           需要用到的一些有效函数
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#include "globals.h"
#include "util.h"

#include "QDebug"

/* 标记输出 */
void printToken(TokenType token, const char* tokenString)
{
    /*根据对应的判断输出判断结果*/
    switch(token)
    {
    case ELSE:
    case IF:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
        fprintf(Syntax_listing, "reserved word: %s\n", tokenString);
        break;
    case LT:fprintf(Syntax_listing,"<\n");break;
    case EQ:fprintf(Syntax_listing,"==\n");break;
    case LPAREN:fprintf(Syntax_listing,"(\n");break;
    case RPAREN:fprintf(Syntax_listing,")\n");break;
    case SEMI:fprintf(Syntax_listing,";\n");break;
    case PLUS:fprintf(Syntax_listing,"+\n");break;
    case MINUS:fprintf(Syntax_listing,"-\n");break;
    case TIMES:fprintf(Syntax_listing,"*\n");break;
    case OVER:fprintf(Syntax_listing,"/\n");break;
    case ENDFILE:fprintf(Syntax_listing,"EOF\n");break;
    case MT:fprintf(Syntax_listing,">\n");break;
    case NEQ:fprintf(Syntax_listing,"!=\n");break;
    case ASSIGN:fprintf(Syntax_listing,"=\n");break;
    case DOT:fprintf(Syntax_listing,",\n");break;
    case LZKH:fprintf(Syntax_listing,"[\n");break;
    case RZKH:fprintf(Syntax_listing,"]\n");break;
    case LDKH:fprintf(Syntax_listing,"{\n");break;
    case RDKH:fprintf(Syntax_listing,"}\n");break;
    case LZS:fprintf(Syntax_listing,"/*\n");break;
    case RZS:fprintf(Syntax_listing,"*/\n");break;
    case ME:fprintf(Syntax_listing,">=\n");break;
    case LE:fprintf(Syntax_listing,"<=\n");break;
    case NUM:
        fprintf(Syntax_listing,"NUM,val= %s\n",tokenString);
        break;
    case ID:
        fprintf(Syntax_listing,"ID, name= %s\n",tokenString);
        break;
    case ERROR:
        fprintf(Syntax_listing,"ERROR: %s\n",tokenString);
            break;
    default:
        fprintf(Syntax_listing,"Unknown token: %d\n",token);
    }
}

/* 为语法树构造创建一个新的语句节点 */
TreeNode * newStmtNode(StmtKind kind)
{
    TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if(t==NULL)
    {
        fprintf(Syntax_listing, "Out of memory error at line %d\n",lineno);
    }
    else
    {
        for(i=0;i<MAXCHILDREN;i++)
        {
            t->child[i]=NULL;
        }
        t->sibling=NULL;
        t->nodekind=StmtK;
        t->kind.stmt=kind;
        t->lineno=lineno;
    }
    return t;
}

/* 为语法树构造创建新的表达式节点 */
TreeNode * newExpNode(ExpKind kind)
{
    TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if(t==NULL)
    {
        fprintf(Syntax_listing, "Out of memory error at line %d\n",lineno);
    }
    else
    {
        for(i=0;i<MAXCHILDREN;i++)
        {
            t->child[i]=NULL;
        }
        t->sibling=NULL;
        t->nodekind=ExpK;
        t->kind.exp=kind;
        t->lineno=lineno;
        t->type=Void;
    }
    return t;
}

/* 分配并复制现有字符串 */
char * copyString(char * s)
{
    int n;
    char * t;
    if(s==NULL)
    {
        return NULL;
    }
    n=strlen(s)+1;
    t=(char *)malloc(n);

    /* 其作用是在内存的动态存储区中分配一个长度为n的连续空间.保存tokenstring */
    if(t==NULL)
    {
        fprintf(Syntax_listing, "Out of memory error at line %d\n",lineno);
    }
    else
    {
        strcpy(t,s);
    /* 该函数是字符串拷贝函数，用来将一个字符串复制到一个字符数组中 */
    /* 例如：strcpy (str1,"china")； 作用是将”China"这个字符串拷贝到str1数组中 */
    }
    return t;
}

/* 用于缩进几层来表示子树层 */
static int indentno =0;
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* 缩进 */
static void printSpaces(void)
{
    int i;
    for(i=0;i<indentno;i++)
    {
        fprintf(Syntax_listing," ");
    }
}

/* 将语法树输出到Syntax_listing文件（tree），用缩进表示子树 */
void printTree(TreeNode * tree)
{
    int i;
    INDENT;

    while(tree!=NULL)
    {
        printSpaces();
        if(tree->nodekind==StmtK)
        {
            switch(tree->kind.stmt)
            {
            case IfK:
                fprintf(Syntax_listing,"If\n");
                break;
            case WhileK:
                fprintf(Syntax_listing,"While\n");
                break;
            case ElseK:
                fprintf(Syntax_listing,"Else\n");
                break;
            case ExpstmtK:
                fprintf(Syntax_listing,"Expression\n");
                break;
            case AssignK:
                switch(tree->type)
                {
                case ArrayUnit:fprintf(Syntax_listing,"Assign to: %s[]\n",tree->attr.name);break;
                default:fprintf(Syntax_listing,"Assign to: %s\n",tree->attr.name);break;
                }
                break;
            case RetK:
                fprintf(Syntax_listing,"return\n");
                break;
            case FunDK:
                fprintf(Syntax_listing,"\n  Function Declaration:\n");
                switch(tree->type)
                {
                case Integer:fprintf(Syntax_listing,"    Return Type: %s \n    Name: %s\n","[int]",tree->attr.name);break;
                case Void:fprintf(Syntax_listing,"    Return Type %s \n    Name: %s \n","[void]",tree->attr.name);break;
                default:break;
                }
                break;
            case ParamK:
                switch(tree->type)
                {
                case Integer: fprintf(Syntax_listing,"Type: %s Name: %s \n","[int]",tree->attr.name);break;
                case IntArray:fprintf(Syntax_listing,"Type: %s Name: %s \n","[int*]",tree->attr.name);break;
                case Void:fprintf(Syntax_listing,"EMPTY\n");break;
                }
                break;
            case VarDK:
                fprintf(Syntax_listing,"Var Declaration: ");
                switch(tree->type)
                {
                case Integer:fprintf(Syntax_listing,"Type: %s  Name: %s \n","[int]",tree->attr.name);break;
                case IntArray:fprintf(Syntax_listing,"Type: %s  Name: %s NUM: %s \n","[int*]",tree->attr.name,tree->attr.val);break;
                case Void:fprintf(Syntax_listing,"Type %s :\n","[void]");break;
                }
                break;
            case LocalDecK:
                fprintf(Syntax_listing,"Local Declarations:\n");
                break;
            case StmtList:
                fprintf(Syntax_listing,"StatementList:\n");
                break;
            case ComK:
                fprintf(Syntax_listing,"Compound:\n");
                break;
            case CallK:
                fprintf(Syntax_listing,"Call Function:%s()\n",tree->attr.name);
                break;
            case ArgK:
                fprintf(Syntax_listing,"Args:\n");
                break;
            case ParamList:
                fprintf(Syntax_listing,"ParamList:\n");
                break;
            case ArgList:
                fprintf(Syntax_listing,"ArgList:\n");
                break;
            default:
                fprintf(Syntax_listing,"Unknown ExpNode kind\n");
                break;
            }
        }

        else if(tree->nodekind=ExpK)
        {
            switch(tree->kind.exp)
            {
            case OpK:
                fprintf(Syntax_listing,"Op: ");
                printToken(tree->attr.op,"\0");
                break;
            case ConstK:
                fprintf(Syntax_listing,"const: %d\n",tree->attr.val);
                break;
            case IdK:
                switch(tree->type)
                {
                case ArrayUnit:fprintf(Syntax_listing,"Id: %s[]\n",tree->attr.name);break;
                default:fprintf(Syntax_listing,"Id: %s\n",tree->attr.name);break;
                }
                break;
            default:
                fprintf(Syntax_listing,"Unknown ExpNode kind\n");
                break;
            }
        }

        else
        {
            fprintf(Syntax_listing,"Unknown node kind\n");
        }

        for(i=0;i<MAXCHILDREN;i++)
        {

            printTree(tree->child[i]);
        }

        tree=tree->sibling;
    }
    UNINDENT;
}
