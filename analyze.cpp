/***************************************************
 * File: analyze.cpp
 * Function: Mini C编译器的语义分析器实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ***************************************************/

#include "mainwindow.h"
#include "globals.h"
#include "symtab.h"
#include "analyze.h"

#include "QString"
#include "QDebug"

/* 可变内存位置的计数器 */
static int location = 0;

/* traverse是一个通用的递归语法树遍历例程:
 * 它对t所指向的树应用前序的预编程和后序的后编程
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{
    while (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i=0; i < MAXCHILDREN; i++){
                traverse(t->child[i],preProc,postProc);
            }
        }
        postProc(t);
        t = t->sibling;
    }
}

/* nullProc是一个从遍历中生成仅前序或仅后序遍历的无操作过程 */
static void nullProc(TreeNode * t)
{
    if (t==NULL) return;
    else return;
}

/* 将存储在int中的标识符插入符号表 */
static void insertNode( TreeNode * t)
{
    switch (t->nodekind)
    {
    case StmtK:
        switch (t->kind.stmt)
        {
        case FunDK:
        case VarDK:
            if (st_lookup(t->attr.name, 0) == -1)
                st_insert(t->attr.name, t->lineno, location++); // 尚未列入表格，因此视为新定义
            else
                st_insert(t->attr.name, t->lineno, 0); // 已经在表中，所以忽略位置，只添加使用的行号
            break;

        case CallK:
            if (st_lookup(t->attr.name, 0) == -1)
                st_insert(t->attr.name,t->lineno,location++); // 尚未列入表格，因此视为新定义
            else
                st_insert(t->attr.name,t->lineno,0); // 已经在表中，所以忽略位置，只添加使用的行号
            break;

        default:
            break;
        }
        break;

    case ExpK:
        switch (t->kind.exp)
        {
        case IdK:
            if (st_lookup(t->attr.name, 0) == -1)
                st_insert(t->attr.name,t->lineno,location++); // 尚未列入表格，因此视为新定义
            else
                st_insert(t->attr.name,t->lineno,0); // 已经在表中，所以忽略位置，只添加使用的行号
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}


/* 通过语法树的前序遍历构建符号表 */
void buildSymtab(TreeNode * syntaxTree)
{
    traverse(syntaxTree,insertNode,nullProc);
    if (!TraceAnalyze)
    {
        fprintf(Analyze_listing,"\nSymbol table:\n\n");
        printSymTab(Analyze_listing);
    }
}

static void typeError(TreeNode * t, char * message)
{
    fprintf(Analyze_listing,"Type error at line %d: %s\n",t->lineno,message);
    Error = TRUE;
}

/* 在单个树节点上执行类型检查 */
static void checkNode(TreeNode * t)
{
    switch (t->nodekind)
    {
    case ExpK:
        switch (t->kind.exp)
        {
        case OpK:
            if ((t->child[0]->type != Integer && t->child[0]->type != ArrayUnit) ||
                            (t->child[1]->type != Integer && t->child[1]->type != ArrayUnit))
                typeError(t, "Op applied to non-integer or non-array unit");

            switch (t->attr.op)
            {
            //+、-、*、/、=的表达式结果都为int类型
            case ASSIGN:
            case PLUS:
            case MINUS:
            case TIMES:
            case OVER:
                t->type = Integer;
                break;

            //>、<、>=、<=、!=、==的表达式结果都为bool类型
            case LE:
            case LT:
            case ME:
            case MT:
            case NEQ:
            case EQ:
                t->type = Boolean;
                break;
            default:
                break;
            }
            break;

        case ConstK:
            t->type = Integer;
            break;

        case IdK:
            if (t->type == ArrayUnit)
                break;
            else
                t->type = Integer;
            if(t->type != Integer && t->type != ArrayUnit)//否则报错
                 typeError(t, "id applied to non-integer or non-array unit");
            break;

        default:
            break;
        }
        break;

    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
            if (t->child[0]->type != Boolean)
                typeError(t->child[0],"if test is not Boolean");
            break;

        case WhileK:
            if (t->child[0]->type != Boolean)
                typeError(t->child[0],"while of non-integer value");
            break;

        case RetK:
            qDebug() << t->child[0]->type;
            if (t->child[0]->type != Integer){
                typeError(t->child[0],"return of non-integer value");}
            break;

        case FunDK:
            if (t->child[0] == NULL) //函数参数列表为空
                break;
            else if (t->child[0]->child[0]->type == Void)//函数参数列表为Void
                break;
            else //多参数时，参数可为int或者数组指针类型
            {
                int i = 0;
                while (t->child[0]->child[i] != NULL)
                {
                    if (t->child[0]->child[i]->type != Integer && t->child[0]->child[i]->type != IntArray)
                    typeError(t->child[0]->child[i], "the parsing of a function argument list is not an integer or integer array");
                    i++;
                }
            }
            break;

        case VarDK:
            t->type = Integer;
            break;

        case CallK:
        {
            lineno = st_lookup(t->attr.name, 1);
            FILE *inFile;
            char lineBuf[256];
            char str[5];
            int bufsize;

            std::string string = cfile.toStdString();
            const char* cf = string.c_str();
            inFile = fopen(cf, "r");

            for (int i = 0; i < lineno; ++i) //获取文件特定第几行的内容
                fgets(lineBuf, 256 - 1, inFile);
            bufsize = strlen(lineBuf);
            for (int i = 0; i < bufsize; i++) //根据函数声明规则，寻找函数的类型
            {
                if (lineBuf[i] == 'i' && lineBuf[i + 1] == 'n' && lineBuf[i + 2] == 't')
                {
                    str[0] = lineBuf[i];
                    str[1] = lineBuf[i + 1];
                    str[2] = lineBuf[i + 2];
                    str[3] = '\0';
                    break;
                }

                if (lineBuf[i] == 'v' && lineBuf[i + 1] == 'o' && lineBuf[i + 2] == 'i' && lineBuf[i + 3] == 'd')
                {
                    str[0] = lineBuf[i];
                    str[1] = lineBuf[i + 1];
                    str[2] = lineBuf[i + 2];
                    str[3] = lineBuf[i + 3];
                    str[4] = '\0';
                    break;
                }
            }
            fclose(inFile);
            if (strcmp(str, "int")  == 0)
                t->type = Integer;
            if (strcmp(str, "void") == 0)
                t->type = Void;
            break;
        }

        default:
            break;
        }
        break;

    default:
        break;

   }
}

/* 通过后置语法树遍历来执行类型检查 */
void typeCheck(TreeNode * syntaxTree)
{
    traverse(syntaxTree,nullProc,checkNode);

}
