#include "globals.h"
#include "util.h"

void printToken( TokenType token, const char* tokenString )
{
    switch (token){
    case IF:
    case ELSE:
    case END:
    case REPEAT:
    case UNTIL:
    case READ:
    case WRITE:
    case WHILE:
    case DO:
    case ENDWHILE:
    case FOR:
    case TO:
    case DOWNTO:
    case ENDDO:fprintf(listing,"reserved word: %s\n",tokenString);break;
    case ASSIGN: fprintf(listing,":=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case EQ: fprintf(listing,"=\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case POWER:fprintf(listing,"^\n"); break;
    case PLUSEQ:fprintf(listing,"+=\n"); break;
    case MINUSEQ:fprintf(listing,"-=\n"); break;
    case TIMESEQ:fprintf(listing,"*=\n"); break;
    case OVEREQ:fprintf(listing,"/=\n"); break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM: fprintf(listing,"NUM, val= %s\n",tokenString);break;
    case ID:fprintf(listing,"ID, name= %s\n",tokenString);break;
    case ERROR:fprintf(listing,"ERROR: %s\n",tokenString);break;
    default: fprintf(listing,"Unknown token: %d\n",token);
  }
}

TreeNode * newStmtNode(StmtKind kind){
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==nullptr)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = nullptr;
        t->sibling = nullptr;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

TreeNode * newExpNode(ExpKind kind){
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==nullptr)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
        for (i=0;i<MAXCHILDREN;i++) t->child[i] = nullptr;
        t->sibling = nullptr;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

char * copyString(char * s){
    unsigned int n;
    char * t;
    if (s==nullptr)
        return nullptr;
    n = strlen(s)+1;
    t = static_cast<char*>(malloc(n));
    if (t==nullptr)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else
        strcpy(t,s);
    return t;
}

static int indentno = 0;

#define INDENT indentno+=2
#define UNINDENT indentno-=2

static void printSpaces(void){
    int i;
    for (i=0;i<indentno;i++)
        fprintf(listing," ");
}

void printTree( TreeNode * tree ){
    int i;
    INDENT;
    while (tree != nullptr) {
        printSpaces();
        if (tree->nodekind==StmtK)
        {
            switch (tree->kind.stmt) {
            case IfK:
                fprintf(listing,"If\n");
                break;
            case RepeatK:
                fprintf(listing,"Repeat\n");
                break;
            case AssignK:
                fprintf(listing,"Assign to: %s\n",tree->attr.name);
                break;
            case PlusEqK:
                fprintf(listing,"PlusEqual to: %s\n",tree->attr.name);
                break;
            case MinusEqK:
                fprintf(listing,"MinusEqual to: %s\n",tree->attr.name);
                break;
            case TimesEqK:
                fprintf(listing,"TimesEqual to: %s\n",tree->attr.name);
                break;
            case OverEqK:
                fprintf(listing,"OverEqual to: %s\n",tree->attr.name);
                break;
            case ReadK:
                fprintf(listing,"Read: %s\n",tree->attr.name);
                break;
            case WriteK:
                fprintf(listing,"Write\n");
                break;
            case WhileK:
                fprintf(listing,"While\n");
                break;
            case DoWhileK:
                fprintf(listing,"Do\n");
                break;
            case ForK:
                fprintf(listing,"For\n");
                break;
            default:
                fprintf(listing,"Unknown ExpNode kind\n");
                break;
            }
        }
        else if (tree->nodekind==ExpK)
        {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing,"Op: ");
                printToken(tree->attr.op,"\0");
                break;
            case ConstK:
                fprintf(listing,"Const: %d\n",tree->attr.val);
                break;
            case IdK:
                fprintf(listing,"Id: %s\n",tree->attr.name);
                break;
            case ToK:
            case DowntoK:
                fprintf(listing,"Conditon: %s\n",tree->attr.name);
                break;
            default:
                fprintf(listing,"Unknown ExpNode kind\n");
                break;
            }
        }
        else
            fprintf(listing,"Unknown node kind\n");

        for (i=0;i<MAXCHILDREN;i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
