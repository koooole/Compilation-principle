#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token;
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_or_opeq_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * while_stmt(void);
static TreeNode * dowhile_stmt(void);
static TreeNode * for_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_expression(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * power(void);

static void syntaxError(const char * message){
    fprintf(listing,"\n>>> ");
    fprintf(listing,"Syntax error at line %d: %s",lineno,message);
    Error = TRUE;
}

static void match(TokenType expected){
    if (token == expected)
        token = getToken();
    else {
        //syntaxError("unexpected token -> ");
        //printToken(token,tokenString);
        fprintf(listing,"      ");
    }
}

TreeNode * stmt_sequence(void){
    TreeNode * t = statement();
    TreeNode * p = t;
    while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL) &&
         (token!=ENDWHILE) && (token!=WHILE) &&
          (token!=ENDDO))
    {
        TreeNode * q;
        match(SEMI);
        q = statement();
        if (q!=nullptr) {
            if (t==nullptr) t = p = q;
            else /* now p cannot be nullptr either */
            {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

TreeNode * statement(void){
    TreeNode * t = nullptr;
    switch (token) {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case ID : t = assign_or_opeq_stmt(); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;
    case WHILE : t = while_stmt(); break;
    case DO : t = dowhile_stmt(); break;
    case FOR : t = for_stmt(); break;
    default : //syntaxError("unexpected token -> ");
              //printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}

TreeNode * if_stmt(void){
    TreeNode * t = newStmtNode(IfK);
    match(IF);
    if (t!=nullptr) t->child[0] = exp();
    if (t!=nullptr) t->child[1] = stmt_sequence();
    if (token==ELSE) {
        match(ELSE);
        if (t!=nullptr) t->child[2] = stmt_sequence();
    }
    match(END);
    return t;
}

TreeNode * repeat_stmt(void){
    TreeNode * t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t!=nullptr) t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t!=nullptr) t->child[1] = exp();
    return t;
}

TreeNode * assign_or_opeq_stmt(void){
    TreeNode * t = newStmtNode(AssignK);
    if ((t!=nullptr) && (token==ID))
      t->attr.name = copyString(tokenString);
    match(ID);
    switch (token) {
    case ASSIGN: match(ASSIGN);break;
    case PLUSEQ: match(PLUSEQ); t->kind.stmt = PlusEqK; break;
    case MINUSEQ: match(MINUSEQ); t->kind.stmt = MinusEqK; break;
    case TIMESEQ: match(TIMESEQ); t->kind.stmt = TimesEqK; break;
    case OVEREQ: match(OVEREQ); t->kind.stmt = OverEqK; break;
    default:syntaxError("unexpected token -> ");
            printToken(token,tokenString);
            token = getToken();
            break;
    }
    if (t!=nullptr) t->child[0] = exp();
    return t;
}

TreeNode * assign_stmt(void){
    TreeNode * t = newStmtNode(AssignK);
    if ((t!=nullptr) && (token==ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    match(ASSIGN);
    if (t!=nullptr)
        t->child[0] = exp();
    return t;
}

TreeNode * read_stmt(void){
    TreeNode * t = newStmtNode(ReadK);
    match(READ);
    if ((t!=nullptr) && (token==ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    return t;
}

TreeNode * write_stmt(void){
    TreeNode * t = newStmtNode(WriteK);
    match(WRITE);
    if (t!=nullptr)
        t->child[0] = exp();
    return t;
}

TreeNode * while_stmt(void){
    TreeNode * t = newStmtNode(WhileK);
    match(WHILE);
    if(t!=nullptr) t->child[0] = exp();
    match(DO);
    if(t!=nullptr) t->child[1] = stmt_sequence();
    match(ENDWHILE);
    return t;
}

TreeNode * dowhile_stmt(void){
    TreeNode * t = newStmtNode(DoWhileK);
    match(DO);
    if(t!=nullptr) t->child[0] = stmt_sequence();
    match(WHILE);
    if(t!=nullptr) t->child[1] = exp();
    return t;
}

TreeNode * for_stmt(void){
    TreeNode * t = newStmtNode(ForK);
    match(FOR);
    if(t!=nullptr)
        t->child[0] = assign_stmt();

    TreeNode * p = nullptr;
    char * pname = copyString(tokenString);
    if(token==TO){
        p = newExpNode(ToK);
        match(TO);
    }
    else if(token==DOWNTO){
        p = newExpNode(DowntoK);
        match(DOWNTO);
    }
    if(p!=nullptr){
        p->attr.name = pname;
        p->child[0] = t->child[0]->child[0];
        p->child[1] = simple_expression();
    }

    if(t!=nullptr)
        t->child[1] = p;
    match(DO);
    if(t!=nullptr)
        t->child[2] = stmt_sequence();
    match(ENDDO);
    return t;
}

TreeNode * exp(void){
    TreeNode * t = simple_expression();
    if ((token==LT)||(token==EQ)) {
        TreeNode * p = newExpNode(OpK);
        if (p!=nullptr) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t!=nullptr)
            t->child[1] = simple_expression();
    }
    return t;
}

TreeNode * simple_expression(void){
    TreeNode * t = term();
    while ((token==PLUS)||(token==MINUS))
    {
        TreeNode * p = newExpNode(OpK);
        if (p!=nullptr) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode * term(void){
    TreeNode * t = factor();
    while ((token==TIMES)||(token==OVER))
    {
        TreeNode * p = newExpNode(OpK);
        if (p!=nullptr) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = factor();
        }
    }
    return t;
}

TreeNode * factor(void){
    TreeNode * t = power();
    while (token==POWER)
    {
        TreeNode * p = newExpNode(OpK);
        if(p!=nullptr){
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = power();
        }
    }
    return t;
}

TreeNode * power(void){
    TreeNode * t = nullptr;
    switch (token) {
    case NUM :
        t = newExpNode(ConstK);
        if ((t!=nullptr) && (token==NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID :
        t = newExpNode(IdK);
        if ((t!=nullptr) && (token==ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN :
        match(LPAREN);
        t = exp();
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

TreeNode * parse(void){
    getTokenInit();
    TreeNode * t;
    token = getToken();
    t = stmt_sequence();
    if (token!=ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}
