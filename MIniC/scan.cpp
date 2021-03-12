/***************************************************
 * File: scan.cpp
 * Function: Mini C 编译器的词法分析部分的实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

#include "QDebug"

/* DFA能到达的各个状态 */
typedef enum{ START,DONE,INNUM,INID,INEQUAL,INLE,INME,INNEQ,INCOMMENT}StateType;

char tokenString[MAXTOKENLEN+1];

#define BUFLEN 256

static char lineBuf[BUFLEN];  // 保存当前行（从源代码文件按行读取得字符串）
static int linepos = 0;       // lineBuf中读取的字符串的当前位置
static int bufsize = 0;       // lineBuf中字符串的当前大小
int EOF_flag = FALSE;         // 更正在EOF上ungetNextChar动作

/* 获取下一个字符 */
static int getNextChar(void)
{
    if (!(linepos < bufsize)){
        lineno++;
        if (fgets(lineBuf,BUFLEN-1,source))
        {

            if (EchoSource)
                fprintf(Syntax_listing,"%4d: %s",lineno,lineBuf);
            bufsize = static_cast<int>(strlen(lineBuf));
            linepos = 0;
            return lineBuf[linepos++];
        }
        else
        {
            EOF_flag = TRUE;
            return EOF;
        }
    }
    else
    {
        return lineBuf[linepos++];
    }
}

/* 将lineBuf中读取的字符串的下标回退一个 */
static void ungetNextChar(void){
    if (!EOF_flag)
        linepos-- ;
}

static struct{
    const char* str;
    TokenType tok;
} reservedWords[MAXRESERVED]  //ELSE,IF,INT,RETURN,VOID,WHILE
   = {
    {"else",ELSE},{"if",IF},{"int",INT},
      {"return",RETURN},{"void",VOID},{"while",WHILE}};

/* 查看当前记号是否为保留字 */
static TokenType reservedLookup (char * s)
{
    int i;
    for (i=0;i<MAXRESERVED;i++)
        if (!strcmp(s,reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}


/* 对扫描的字符进行匹配判断（获取token） */
TokenType getToken(void)
{
    int tokenStringIndex = 0;  // 记号字符串下标
    TokenType currentToken;    // 保存当前记号（作为返回值）
    StateType state = START;   // 当前状态（总是从START开始）
    int save;                  // 是否保存到tokenString的标志

    while (state != DONE)
    {
        int c = getNextChar();
        save = TRUE;
        switch (state)
        {
        case START:
            if (isdigit(c))
                state = INNUM;
            else if (isalpha(c))
                state = INID;
            else if (c == '=')
                state = INEQUAL;
            else if (c == '<')
                state = INLE;
            else if (c == '>')
                state = INME;
            else if (c== '!')
                state = INNEQ;
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = FALSE;
            else if (c == '/')
            {
                if(getNextChar()!='*')
                {
                    ungetNextChar();
                    state = DONE;
                    currentToken = OVER;
                    break;
                }
                else
                {
                    save = FALSE;
                    state = INCOMMENT;
                }
            }

            else
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case '[':
                    currentToken=LZKH;
                    break;
                case ']':
                    currentToken=RZKH;
                    break;
                case '{':
                    currentToken=LDKH;
                    break;
                case '}':
                    currentToken=RDKH;
                    break;
                case ',':
                    currentToken=DOT;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;

        case INCOMMENT:
            save = FALSE;
            if (c == EOF)
            {
                state = DONE;
                currentToken = ERROR;
            }
            else if(c=='*')
            {
                if(getNextChar()=='/')
                {
                    state = START;
                }
                else
                {
                    ungetNextChar();
                }
            }
            break;

        case INNEQ:
            state=DONE;
            if(c=='=')
                currentToken=NEQ;
            else
            {
                ungetNextChar();
                save=FALSE;
                currentToken=ERROR;
            }
            break;

        case INEQUAL:
            state = DONE;
            if (c == '=')
                currentToken = EQ;
            else
            {
                ungetNextChar();
                currentToken = ASSIGN;
            }
            break;

        case INNUM:
            if (!isdigit(c))
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;

        case INID:
            if (!isalpha(c))
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;

        case INLE:
            state = DONE;
            if(c== '=')
                currentToken = LE;
            else
            {
                ungetNextChar();
                currentToken = LT;
            }
            break;

        case INME:
            state = DONE;
            if(c== '=')
                currentToken = ME;
            else
            {
                ungetNextChar();
                currentToken = MT;
            }
            break;

        case DONE:

        default:
            fprintf(Syntax_listing,"Scanner Bug: state= %d\n",state);
            state = DONE;
            currentToken = ERROR;
            break;
        }

        /* 保存非注释等内容 */
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = (char) c;

        /* 当前字符串结尾 */
        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }

    if (TraceScan)
    {
        fprintf(Syntax_listing,"\t%d: ",lineno);
        printToken(currentToken,tokenString);
    }

    genToken(currentToken, tokenString);
    // qDebug() << currentToken;
    return currentToken;

} /* end getToken */

// 生成词法分析结果
void genToken(TokenType currentToken, const char* tokenString){

    switch (currentToken) {
    case 0: break;
    case 1: break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        fprintf(Token_listing,"  %-14s  %-8s\n",tokenString,"Reserve Word");
        break;
    case 8:
        fprintf(Token_listing,"  %-14s  %-8s\n",tokenString,"Token(ID)");
        break;
    case 9:
        fprintf(Token_listing,"  %-14s  %-8s\n",tokenString,"Token(NUM)");
        break;
    default:
        fprintf(Token_listing,"  %-14s  %-8s\n",tokenString,"Special Symbol");
        break;
    }
}
