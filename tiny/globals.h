#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 14

typedef enum 
    /* book-keeping tokens */
   {ENDFILE,ERROR,
    /* reserved words */
    IF,ELSE,END,REPEAT,UNTIL,READ,WRITE,WHILE,DO,ENDWHILE,FOR,TO,DOWNTO,ENDDO,
    /* multicharacter tokens */
    ID,NUM,
    /* special symbols */
    ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI,POWER,PLUSEQ,MINUSEQ,TIMESEQ,OVEREQ
   } TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,RepeatK,AssignK,ReadK,WriteK,PlusEqK,MinusEqK,TimesEqK,OverEqK,WhileK,DoWhileK,ForK} StmtKind;
typedef enum {OpK,ConstK,IdK,ToK,DowntoK} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode{
    struct treeNode * child[MAXCHILDREN];
    struct treeNode * sibling;
    int lineno;
    NodeKind nodekind;
    union { StmtKind stmt; ExpKind exp;} kind;
    union {
        TokenType op;
        int val;
        char * name;
    } attr;
    ExpType type; /* for type checking of exps */
} TreeNode;


extern int EchoSource;

extern int TraceScan;

extern int TraceParse;

extern int TraceAnalyze;

extern int TraceCode;

extern int Error; 
#endif
