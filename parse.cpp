/***************************************************
 * File: parse.cpp
 * Function: Mini C 编译器的解析部分实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#include "QDebug"

static TokenType token; // 当前扫描到的记号

/* 递归调用的函数原型 */
static TreeNode * program(void);
static TreeNode * declaration_list(void);
static TreeNode * declaration(void);
static TreeNode * params(void);
static TreeNode * param_list(void);
static TreeNode * param(void);
static TreeNode * compound_stmt(void);
static TreeNode * local_declarations(void);
static TreeNode * statement_list(void);
static TreeNode * statement(void);
static TreeNode * expression_stmt(void);
static TreeNode * if_stmt(void);
static TreeNode * while_stmt(void);
static TreeNode * return_stmt(void);
static TreeNode * expression(void);
static TreeNode * var(void);
static TreeNode * simple_expression(void);
static TreeNode * additive_expression(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * args(void);
static TreeNode * arg_list(void);

/* 语法错误输出 */
static void syntaxError(char * message)
{
    fprintf(Syntax_listing,"\n>>> ");
    fprintf(Syntax_listing,"Syntax error at line %d: %s",lineno,message);
    Error = TRUE;
}

/* 匹配读取的字符 */
static void match(TokenType expected)
{
    //qDebug() << "expected " << expected << "token " << token;
    if(token==expected)
    {
        token=getToken( );
    }
    else
    {
        syntaxError("(match error)unexpected token -> ");
        printToken(token,tokenString);
        fprintf(Syntax_listing,"       ");
    }
}

/* ----------------进行语法分析，构建语法树----------------- */

/*
 program -> declaration-list
 分析的开始函数
*/
TreeNode * program(void){
    TreeNode * t = declaration_list();
    return t;
}

/*
 declaration_list -> declaration_list declaration | declaration
 消除左递归：declaration_list -> declaration { declaration }
 first(declartion_list) = { INT, VOID }
*/
TreeNode * declaration_list(void)
{
    TreeNode * t= declaration();
    TreeNode * p= t;
    while ((token==INT) || (token==VOID) )
    {
        TreeNode *q = declaration();
        if (q!=NULL) {
            if (t==NULL)
                t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

/*
 declaration -> var-declaration | fun-declaration
 var-declaration -> type-specifier ID; | type-specifier ID[NUM];
 type-specifier -> int | void
 fun-declaration -> type-sype-specifier ID(params) | compound-stmt
 first(declartion) = { INT, VOID }
*/
TreeNode * declaration(void)
{
    TreeNode * t = NULL;

    switch (token)
    {
    case VOID :
    case INT :
        t =  newStmtNode(DecK);
        if(token == INT)
            t->type = Integer;
        else
            t->type = Void;

        match(token);

        switch (token)
        {
        case ID:
            char * string;
            string = copyString(tokenString);
            t->attr.name = string;
            t->kind.stmt = VarDK;
            match(ID);
            switch (token)
            {
            case LZKH: // 数组
                t->kind.stmt = VarDK;
                t->type = IntArray;
                //qDebug() << string;
                t->attr.name = string;
                match(LZKH);
                t->attr.val = int(copyString(tokenString));
                match(NUM);
                match(RZKH);
                match(SEMI);
                break;
            case LPAREN:
                t->kind.stmt = FunDK;
                match(LPAREN);
                t->child[0] = params();
                match(RPAREN);
                t->child[1] = compound_stmt();
                break;
            default:  match(SEMI);break;
            }
            break;

        default:
            syntaxError("unexpected token -> ");
            printToken(token,tokenString);
            token = getToken();
            break;
        }
        break;

    default:
        syntaxError("unexpected token -> ");
        printToken(token,tokenString);
        token = getToken();
        break;
    }

    return t;
}

/*
 params -> params-list | void
*/
TreeNode * params(void)
{
    TreeNode * t = NULL;
    if(token == VOID)
    {
        match(token);
        t = newStmtNode(ParamList);
        t->child[0] = newStmtNode(ParamK);
        t->child[0]->type = Void;
    }
    else if(token == RPAREN)
        t=NULL;
    else
    {
        t = param_list();
    }
    return t;
}

/*
 param-list -> param-list, param | param
 消除左递归：param-list -> param {, param}
*/
TreeNode * param_list(void)
{
    TreeNode * t = newStmtNode(ParamList);
    int i = 1;
    t->child[0] = param();

    while(token != RPAREN)
    {
        match(DOT);
        t->child[i] = param();
        i++;
    }
    return t;
}

/*
 param -> type-specifier ID | type-specifier ID[]
 type-specifier -> int | void
*/
TreeNode * param(void)
{
    TreeNode * t = NULL;
    match(INT);
    t= newStmtNode(ParamK);
    t->type=Integer;
    t->attr.name=copyString(tokenString);
    match(ID);
    if(token == LZKH)
    {
        t->type=IntArray;
        match(LZKH);
        match(RZKH);
    }
    return t;
}

/*
 compound-stmt -> {lacal-declarations statement-list }
*/
TreeNode * compound_stmt(void)
{
    TreeNode * t =  newStmtNode(ComK);
    match(LDKH);
    t->child[0] = local_declarations();
    t->child[1] = statement_list();

    match(RDKH);
    return t;
}

/*
 local-declarations -> local-declarations var-declaration | empty
 var-declaration -> type-specifier ID; | type-specifier ID[NUM];
 type-specifier -> int | void
 消除左递归：local-declarations -> empty {var-declaration}
 first = {INT, VOID}
*/
TreeNode * local_declarations(void)
{
    TreeNode * t = newStmtNode(LocalDecK);
    int i=0;
    while(token == INT || token == VOID)
    {

        t->child[i] = declaration();
        i++;
    }
    return t;
}

/*
 statement-list -> statement-list statement | empty
 消除左递归：statement-list -> {statement}
*/
TreeNode * statement_list(void)
{
    TreeNode * t = newStmtNode(StmtList);
    int i=0;
    while(token != RDKH)
    {
        t->child[i] =statement();
        i++;
    }
    return t;
}

/*
 statement ->expression-stmt | compound-stme | if-stmt(指导书为selection-stmt) |
             while-stmt(指导书为iteration-stmt） | return_stmt
 first(statement) = {IF,WHILE,ID,SEMI,RETURN,LDKH}
*/
TreeNode * statement(void)
{
    TreeNode * t ;
    switch (token) {

    case IF : t = if_stmt(); break;
    case WHILE : t = while_stmt(); break;
    case ID :
    case SEMI:t = expression_stmt(); break;
    case RETURN : t = return_stmt(); break;
    case LDKH : t=compound_stmt();break;
    default :
        syntaxError("unexpected token -> ");
        printToken(token,tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*
 expression-stmt = expression ; | ;
*/
TreeNode * expression_stmt(void)
{
    TreeNode * t = newStmtNode(ExpstmtK);
    if(token == SEMI)
        match(SEMI);
    else
    {
        t = expression();
        match(SEMI);
    }
    return t;
}

/*
 if-stmt(selection-stmt) -> if(expression) statement
                            | if(expression) statement else statement
  消除左公因子：if-stmt(selection-stmt) -> if(expression) statement [else statement]
*/
TreeNode * if_stmt(void)
{
    TreeNode * t = newStmtNode(IfK);
    if(t!=NULL)
    {
        match(IF);
        match(LPAREN);
        t->child[0] = expression();

        match(RPAREN);
        t->child[1] = statement();
        if (token==ELSE)
        {
            match(ELSE);
           if (t!=NULL) t->child[2] = newStmtNode(ElseK);
           t->child[2]->child[0] = statement();
        }	}
    return t;
}

/*
 while-stmt(iteration-stmt) -> while(expression)statement
*/
TreeNode * while_stmt(void)
{
    TreeNode * t = newStmtNode(WhileK);
    match(WHILE);
    match(LPAREN);
    if (t!=NULL) t->child[0] = expression();
    match(RPAREN);
    if (t!=NULL) t->child[1] = statement();
    return t;
}

/*
 return-stmt -> return ; | return expression;
 消除左公因子：return-stmt -> return (; | expression;)
*/
TreeNode * return_stmt(void)
{
    TreeNode * t = newStmtNode(RetK);
    if(token == RETURN)
        match(RETURN);

    if(token == SEMI)
        match(SEMI);
    else
    {
        t->child[0] = expression();
        match(SEMI);
    }
    return t;
}

/*
 expression -> var=expression | simple-expression
*/
TreeNode * expression(void)
{
    TreeNode * t = simple_expression();
    return t;
}

/*
 var = ID | ID[expression]
 消除左公因子：var = ID[[expression]]
*/
TreeNode* var(void)
{
    TreeNode* t = newExpNode(IdK);
    if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    if(token == LZKH)
    {
        match(token);
        t->type = ArrayUnit;
        t->child[0] = expression();
        match(RZKH);
    }
    return t;
}

/*
 simple-expression -> additive-expression relop additive-expression | additive-expression
 relop -><=|<|>|>=|==|!=
 消除左公因子：simple-expression -> additive-expression [relop additive-expression]
*/
TreeNode * simple_expression(void)
{
    TreeNode * t = additive_expression();
    if(t!=NULL){
        if (token == LT || token == LE|| token == MT || token == ME||token ==EQ||token ==NEQ)
        {
            TreeNode * p = newExpNode(OpK);
            if(p!=NULL)
            {
                p->attr.op = token;
                p->child[0] = t;
                match(token);
                p->child[1] = additive_expression();
                t=p;
            }
        }
    }
    return t;
}

/*
 additive-expression -> additive-expression adddop term | term
 addop -> +|-
 消除左递归：additive-expression -> term {addop term}
*/
TreeNode* additive_expression(void)
{
    TreeNode * t = term();
    while(token == PLUS || token == MINUS)
    {
        TreeNode * p = newExpNode(OpK);
        p->attr.op = token;
        p->child[0] = t;
        match(token);
        p->child[1] = term();
        t = p;
    }
    return t;
}

/*
 term -> term mulop factor | faction
 mulop-> *|/
 消除左递归：term -> factor {mulop factor}
*/
TreeNode * term(void)
{
    TreeNode * t = factor();
    while ((token==TIMES)||(token==OVER))
    {
        TreeNode * p = newExpNode(OpK);
        if (p!=NULL) {
            p->child[0] = t;
            p->attr.op = token;

            match(token);
            p->child[1] = factor();
            t = p;
        }
    }
    return t;
}

/*
 factor -> (expression) | var | call | NUM
 call -> ID(args)
 first(factor) = {LPAREN, ID, NUM}
*/
TreeNode * factor(void)
{
    TreeNode * t = NULL;
    switch (token)
    {
    case NUM :
        t = newExpNode(ConstK);
        if ((t!=NULL) && (token==NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID :
        t = var();
        if (token == ASSIGN)
        {
            TreeNode* p = newStmtNode(AssignK);
            p->attr.name = t->attr.name;
            match(token);
            p->child[0] = expression();
            t = p;
        }
        if (token == LPAREN )
        {
            TreeNode * p = newStmtNode(CallK);
            p->attr.name = t->attr.name;
            t=p;
            match(token);
            p->child[0] = args();
            match(RPAREN);
        }
        break;
    case LPAREN :
        match(LPAREN);
        t = expression();
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token,tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*
 args -> arg-list | empty
*/
TreeNode * args(void)
{
    TreeNode * t = newStmtNode(ArgList);
    if(token != RPAREN)
    {
        t->child[0] = arg_list();
        return t;
    }else
        return NULL;
}

/*
 arg-list -> arg-list,expreesion | expression
 消除左递归：arg-list -> expression {,expression}
*/
TreeNode * arg_list(void)
{
    TreeNode * t = newStmtNode(ArgK);
    int i = 1;
    if(token != RPAREN)
        t->child[0] = expression();
    while(token!=RPAREN)
    {
        match(DOT);
        t->child[i] = expression();
        i++;
    }
    return t;
}

/* 解析函数 */
TreeNode * parse(void)
{
    TreeNode * t;
    token = getToken();
    t = program(); // 开始函数
    if (token!=ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}
