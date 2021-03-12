/****************************************************
 * File: cgen.cpp
 * Function: Mini C编译器的代码生成器实现(为TM机器生成代码)
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

#include "QDebug"

/* tmpOffset是temps的内存偏移量
 * 每当存储一个temp时，它就递减，当再次加载时，它就递增
 */
static int tmpOffset = 0;

/* 通过遍历树递归生成代码的原型 */
static void cGen (TreeNode * tree);

/* 在语句节点生成代码 */
static void genStmt( TreeNode * tree)
{
    TreeNode * p1, * p2, * p3;
    int savedLoc1,savedLoc2,currentLoc;
    int loc;
    switch (tree->kind.stmt)
    {
    case IfK :
        if (TraceCode) emitComment("-> if") ;
        p1 = tree->child[0] ;
        p2 = tree->child[1] ;
        p3 = tree->child[2] ;

         /* 为测试表达式生成中间代码 */
        cGen(p1);
        savedLoc1 = emitSkip(1) ;
        emitComment("if: jump to else belongs here");

         /* if上递归 */
        cGen(p2);
        savedLoc2 = emitSkip(1) ;
        emitComment("if: jump to end belongs here");
        currentLoc = emitSkip(0) ;
        emitBackup(savedLoc1) ;
        emitRM_Abs("JEQ",ac,currentLoc,"if: jmp to else");
        emitRestore() ;

         /* else上递归 */
        cGen(p3);
        currentLoc = emitSkip(0) ;
        emitBackup(savedLoc2) ;
        emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
        emitRestore() ;

        if (TraceCode) emitComment("<- if") ;
        break;

    case LocalDecK:
        if (TraceCode) emitComment("-> local declaration");
        for (int i = 0; i < MAXCHILDREN; i++)
        {
            p1 = tree->child[i];
            if (p1 != NULL)
            cGen(p1);
        }
        if (TraceCode)  emitComment("<- local declaration");
        break;

    case RetK:
        if (TraceCode) emitComment("-> return ");
        p1 = tree->child[0];

        /* 表达式生成中间代码 */
        cGen(p1);

        /* 保存值 */
        loc = st_lookup(p1->attr.name, 0);
        emitRM("ST", ac, loc, gp, "return: store value");
        if (TraceCode)  emitComment("<- return");
        break;

    case AssignK:
        if (TraceCode) emitComment("-> assign") ;

         /* 表达式生成中间代码 */
        cGen(tree->child[0]);

         /* 保存值 */
        loc = st_lookup(tree->attr.name, 0);
        emitRM("ST",ac,loc,gp,"assign: store value");

        if (TraceCode) emitComment("<- assign") ;
        break; /* assign_k */

    case ElseK:
        if (TraceCode) emitComment("-> else");
        p1 = tree->child[0];
        cGen(p1);
        if (TraceCode)  emitComment("<- else");
        break;

    case WhileK:
        if (TraceCode) emitComment("-> while");
        p1 = tree->child[0];
        p2 = tree->child[1];

        savedLoc1 = emitSkip(1);

        /* 为判断语句表达式生成中间代码 */
        cGen(p1);
        emitRM_Abs("JEQ", ac, savedLoc1, "while: jump after body comes back here");

        /* while为True执行语句进行递归生成中间代码 */
        cGen(p2);
        emitComment("while: jmp back to body");

        if (TraceCode)  emitComment("<- while");
        break;

    case FunDK:
        if (TraceCode) emitComment("-> function declaration");

        /* 保存值 */
        loc = st_lookup(tree->attr.name, 0);
        emitRM("ST", ac, loc, gp, "store function");
        p1 = tree->child[0];
        p2 = tree->child[1];

        /* 递归调用生成中间代码 */
        cGen(p1);
        cGen(p2);

        if (TraceCode)  emitComment("<- function declaration");
        break;

    case VarDK:
        if (TraceCode) emitComment("-> var declaration");

        loc = st_lookup(tree->attr.name, 0);
        emitRM("ST",ac,loc,gp,"read: store value");
        if (TraceCode)  emitComment("<- var declaration");
        break;

    case StmtList:
        if (TraceCode) emitComment("-> statement list");

        /* 递归调用生成中间代码 */
        for (int i = 0; i < MAXCHILDREN; i++)
        {
            p1 = tree->child[i];
            cGen(p1);
        }
        if (TraceCode)  emitComment("<- statement list");
        break;

    case ParamList:
        if (TraceCode) emitComment("-> param list");
        p1 = tree->child[0];
        cGen(p1);

        /* 递归调用生成中间代码 */
        for (int i = 1; i < MAXCHILDREN; i++)
        {
            p1 = tree->child[i];
            cGen(p1);
        }
        if (TraceCode)  emitComment("<- param list");
        break;

    case CallK:
        if (TraceCode) emitComment("-> call");

        // qDebug() << tree->attr.name << strlen(tree->attr.name);

        /* 若为input函数调用,读入寄存器 */
        if (strcmp(tree->attr.name, "input") == 0)
        {
            emitRO("IN", ac, 0, 0, "input integer value");
            loc = st_lookup(tree->attr.name, 0);
            emitRM("ST", ac, loc, gp, "input: store value");
        }

        /* 若为output函数调用,从寄存器取出 */
        else if (strcmp(tree->attr.name, "output") == 0)
        {
            cGen(tree->child[0]);
            emitRO("OUT", ac, 0, 0, "output integer value");
        }

        else
        {
            p1 = tree->child[0];
            cGen(p1);
            loc = st_lookup(tree->attr.name, 0);
            emitRM("ST", ac, loc, gp, "call: store value");
        }
        if (TraceCode)  emitComment("<- call");
        break;

    case ArgList:
        if (TraceCode) emitComment("-> arglist");
        p1 = tree->child[0];
        cGen(p1);
        if (TraceCode)  emitComment("<- arglist");
        break;

    case ParamK:
        if (TraceCode) emitComment("-> param");
        if (tree->type == Void)
            break;
        else
        {
            loc = st_lookup(tree->attr.name, 0);
            emitRM("ST", ac, loc, gp, "param: store the value of a parameter list");
        }
        if (TraceCode)  emitComment("<- param");
        break;

    case ComK:
        if (TraceCode) emitComment("-> compound");
        p1 = tree->child[0];
        p2 = tree->child[1];
        cGen(p1);
        cGen(p2);
        if (TraceCode)  emitComment("<- compound");
        break;

    case ArgK:
        if (TraceCode) emitComment("-> arg");
        p1 = tree->child[0];
        cGen(p1);
        if (TraceCode)  emitComment("<- arg");
        break;

    default:
        break;
    }
} /* genStmt */

/* 在表达式节点生成代码 */
static void genExp( TreeNode * tree)
{
    int loc;
    TreeNode * p1, * p2;
    switch (tree->kind.exp)
    {
    case ConstK :
        if (TraceCode) emitComment("-> Const") ;

       /* 使用LDC加载整型常量生成中间代码 */
       emitRM("LDC",ac,tree->attr.val,0,"load const");
       if (TraceCode)
           emitComment("<- Const") ;
       break; /* ConstK */
    
    case IdK :
        if (TraceCode) emitComment("-> Id") ;
        if (tree->type == ArrayUnit)
        {
            /* 使用LD加载数组类型生成中间代码 */
            cGen(tree->child[0]);
            loc = st_lookup(tree->child[0]->attr.name, 0);
            emitRM("LD", ac, loc, gp, "load array index value");
            loc = st_lookup(tree->attr.name, 0);
            emitRM("LD", ac, loc, gp, "load array unit value");
        }
        else
        {
            loc = st_lookup(tree->attr.name, 0);
            emitRM("LD", ac, loc, gp, "load id value");
        }

        if (TraceCode) emitComment("<- Id") ;
        break; /* IdK */

    case OpK :
        if (TraceCode) emitComment("-> Op") ;
        p1 = tree->child[0];
        p2 = tree->child[1];

         /* 生成代码: ac = left arg */
        cGen(p1);

         /* 生成代码: push left operand */
        emitRM("ST",ac,tmpOffset--,mp,"op: push left");

         /* 生成代码: ac = right operand */
        cGen(p2);

         /* 加载左操作数 */
        emitRM("LD",ac1,++tmpOffset,mp,"op: load left");

        switch (tree->attr.op)
        {
        case PLUS :
            emitRO("ADD",ac,ac1,ac,"op +");
            break;
        case MINUS :
            emitRO("SUB",ac,ac1,ac,"op -");
            break;
        case TIMES :
            emitRO("MUL",ac,ac1,ac,"op *");
            break;
        case OVER :
            emitRO("DIV",ac,ac1,ac,"op /");
            break;
        case LT :
            emitRO("SUB",ac,ac1,ac,"op <") ;
            emitRM("JLT",ac,2,pc,"br if true") ;
            emitRM("LDC",ac,0,ac,"false case") ;
            emitRM("LDA",pc,1,pc,"unconditional jmp") ;
            emitRM("LDC",ac,1,ac,"true case") ;
            break;
        case MT :
            emitRO("SUB", ac, ac1, ac, "op >");
            emitRM("JGT", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case ME:
             emitRO("SUB", ac, ac1, ac, "op >=");
             emitRM("JGE", ac, 2, pc, "br if true");
             emitRM("LDC", ac, 0, ac, "false case");
             emitRM("LDA", pc, 1, pc, "unconditional jmp");
             emitRM("LDC", ac, 1, ac, "true case");
        case LE:
            emitRO("SUB", ac, ac1, ac, "op <=");
            emitRM("JLE", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case EQ :
            emitRO("SUB",ac,ac1,ac,"op ==") ;
            emitRM("JEQ",ac,2,pc,"br if true");
            emitRM("LDC",ac,0,ac,"false case") ;
            emitRM("LDA",pc,1,pc,"unconditional jmp") ;
            emitRM("LDC",ac,1,ac,"true case") ;
            break;
        case NEQ:
            emitRO("SUB", ac, ac1, ac, "op !=");
            emitRM("JNE", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        default:
            emitComment("BUG: Unknown operator");
            break;
        } /* case op */
        if (TraceCode)
            emitComment("<- Op") ;
        break; /* OpK */

    default:
        break;
    }
} /* genExp */

/* 通过遍历树递归生成代码 */
static void cGen( TreeNode * tree)
{
    if (tree != NULL)
    {
        switch (tree->nodekind)
        {
        case StmtK:
            genStmt(tree);
            break;
        case ExpK:
            genExp(tree);
            break;
        default:
            break;
        }
        cGen(tree->sibling);
    }
}

/********************/
/* 代码生成器的主要功能 */
/********************/

/* 通过遍历语法树生成代码文件的代码。
 * 第二个参数(codefile)是代码文件的文件名，用于将文件名打印为代码文件中的注释
 */
void codeGen(TreeNode * syntaxTree, char * codefile)
{
    char * s = (char *)malloc(strlen(codefile)+7);
    strcpy(s,"File: ");
    strcat(s,codefile);
    emitComment("Mini C Compilation to TM Code");
    emitComment(s);

   /* 生成标准之前 */
    emitComment("Standard prelude:");
    emitRM("LD",mp,0,ac,"load maxaddress from location 0");
    emitRM("ST",ac,0,ac,"clear location 0");
    emitComment("End of standard prelude.");

   /* 为Mini C 程序生成代码 */
    cGen(syntaxTree);

   /* 结束 */
    emitComment("End of execution.");
    emitRO("HALT",0,0,0,"");

}
