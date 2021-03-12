/***************************************************
 * File: global.h
 * Function: Mini C 编译器的全局变量定义头文件
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* 保留字的个数 */
#define MAXRESERVED 6

typedef enum
{
    ENDFILE,ERROR,
    /* 保留字 */
    ELSE,IF,INT,RETURN,VOID,WHILE,
    /* 标记符号 */
    ID,NUM,
    /* 专用符号 */
    PLUS,MINUS,TIMES,OVER,LT,MT,EQ,NEQ,ASSIGN,SEMI,DOT,LPAREN,RPAREN,LZKH,RZKH,\
    LDKH,RDKH,LZS,RZS,ME,LE
} TokenType;

extern int lineno;            // 源文件中的行数
extern FILE* source;          // 读取源代码的句柄
extern FILE* Syntax_listing;  // 用于保存生成的语法树文本文件（保存到tree文件，包括报错的情况）的句柄
extern FILE* Token_listing;   // 用于保存生成的词法分析文本文件（保存到token文件）的句柄
extern FILE* Analyze_listing; // 用于保存生成的语义分析结果文本文件（保存到symtab文件，包括报错的情况）的句柄
extern FILE* code;            // 用于保存生成的TM模拟器的代码文本文件的句柄

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,LocalDecK,RetK,ExpstmtK,AssignK,ElseK,WhileK,FunDK,VarDK,DecK,\
              StmtList,ParamList,CallK,ArgList,ParamK,ComK,ArgK} StmtKind;
typedef enum {OpK,ConstK,IdK} ExpKind;

/* 用于类型检查 */
typedef enum {Void,Integer,Boolean,IntArray,ArrayUnit} ExpType;

#define MAXCHILDREN 5
typedef struct treeNode
   {
     struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { StmtKind stmt;
             ExpKind exp;} kind;

     struct attrs{
         TokenType op;
         int val;
         char * name;
     } attr;

     ExpType type;
   } TreeNode;

extern int EchoSource;   // EchoSource = TRUE时，源程序在解析过程中以行号回显到listing文件
extern int TraceScan;    // TraceScan = TRUE时，当分析器识别出每个token时，将token信息打印到listing文件中
extern int TraceParse;   // TraceParse = TRUE时，语法树以线性形式打印到listing文件中(使用缩进表示子树)
extern int TraceAnalyze; // TraceAnalyze = TRUE时，将符号表插入和查找报告打印到listing文件
extern int TraceCode;    // TraceCode = TRUE时，在代码生成时将注释写入TM代码文件

extern int Error;        // Error为真时，如果发生错误，防止进一步通过
extern int EOF_flag;     // 更正在EOF上ungetNextChar动作


#endif // GLOBALS_H
