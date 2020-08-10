#include "globals.h"
#include "util.h"
#include "scan.h"

typedef enum{ START,INASSIGN,INCOMMENT,INNUM,INID,INOPERATOR,DONE}StateType;

char tokenString[MAXTOKENLEN+1];

#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

static int getNextChar(void)
{
    if (!(linepos < bufsize)){
        lineno++;
        if (fgets(lineBuf,BUFLEN-1,source)){
            if (EchoSource)
                fprintf(listing,"%4d: %s",lineno,lineBuf);
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
        return lineBuf[linepos++];
}

static void ungetNextChar(void){
    if (!EOF_flag) linepos-- ;
}

static struct{
    const char* str;
    TokenType tok;
} reservedWords[MAXRESERVED]
   = {{"if",IF},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE},{"while",WHILE},{"do",DO},
      {"endwhile",ENDWHILE},{"for",FOR},{"to",TO},
      {"downto",DOWNTO},{"enddo",ENDDO}};

static TokenType reservedLookup (char * s)
{
    int i;
    for (i=0;i<MAXRESERVED;i++)
        if (!strcmp(s,reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}

static bool isoperator(int c){
    return (c=='+' || c=='-' || c=='*' || c=='/');
}

void getTokenInit(void)
{
    lineno = 0;
    linepos = 0;
    bufsize = 0;
    EOF_flag = FALSE;
}

TokenType getToken(void)
{
    int tokenStringIndex = 0;
    TokenType currentToken = ERROR;
    StateType state = START;
    int save;
    int optoken = 0;
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
            else if (c == ':')
                state = INASSIGN;
            else if (isoperator(c))
            {
                optoken = c;
                state = INOPERATOR;
            }
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = FALSE;
            else if (c == '{')
            {
                save = FALSE;
                state = INCOMMENT;
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
                case '=':
                    currentToken = EQ;
                    break;
                case '<':
                    currentToken = LT;
                    break;
                case '^':
                    currentToken = POWER;
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
                currentToken = ENDFILE;
            }
            else if (c == '}')
                state = START;
                break;
        case INASSIGN:
            state = DONE;
            if (c == '=')
                currentToken = ASSIGN;
            else
            { /* backup in the input */
                 ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case INOPERATOR:
            state = DONE;
            if (c == '=') {
             /* Binocular operator : += -= *= /= */
                switch (optoken) {
                case '+': currentToken = PLUSEQ; break;
                case '-': currentToken = MINUSEQ; break;
                case '*': currentToken = TIMESEQ; break;
                case '/': currentToken = OVEREQ; break;
                default: currentToken = ERROR; break;
                }
            }
            else {
             /* backup in the input */
                 ungetNextChar();
                 save = FALSE;
             /* Monocular operator : + - * / */
                switch (optoken) {
                case '+': currentToken = PLUS; break;
                case '-': currentToken = MINUS; break;
                case '*': currentToken = TIMES; break;
                case '/': currentToken = OVER; break;
                default: currentToken = ERROR; break;
                }
            }
            break;
        case INNUM:
            if (!isdigit(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID:
            if (!isalpha(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;
        case DONE:
        default: /* should never happen */
            fprintf(listing,"Scanner Bug: state= %d\n",state);
            state = DONE;
            currentToken = ERROR;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = char(c);
        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }

    if (TraceScan) {
        fprintf(listing,"\t%d: ",lineno);
        printToken(currentToken,tokenString);
    }
    return currentToken;
} /* end getToken */

