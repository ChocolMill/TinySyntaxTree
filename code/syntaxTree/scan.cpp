#include "globals.h"
#include "util.h"
#include "scan.h"
#include <QDebug>
#include <string>

/* states in scanner DFA */
typedef enum
{
    START, INASSIGN, INCOMMENT, INNUM, INID, DONE
}
StateType;

/* lexeme of identifier or reserved word */
/* MAXTOKENLEN is the maximum size of a token 定义在 scan.h 中*/
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

void reset()
{
    lineno = 0;
    linepos = 0;
    bufsize = 0;
    EOF_flag = FALSE;
}

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
/* 从 lineBuf 中获取下一个非空白字符。如果 lineBuf 耗尽，则读取新的一行源代码到 lineBuf 中。 */
/* debugMsg 定义在 globals.h 中。 */
/* 当 EchoSource 被设置为 TRUE（1）时，编译器或解释器在解析源代码的过程中，会将当前处理的源代码行以及行号一起输出到列表文件中。
 * 这有助于开发者或用户了解编译器在处理源代码时的具体位置。
 * 当 EchoSource 被设置为 FALSE（0）时，源代码不会被回显到列表文件中。
*/
static int getNextChar(void)
{
    if (!(linepos < bufsize))
    {
        lineno++;   // linepos 不小于 bufsize，说明 lineBuf 已经读完，需要读取新的一行
        if (fgets(lineBuf, BUFLEN - 1, source))     // 从 source 文件中读取新的一行到 lineBuf
        {
            qDebug() << lineno<< " (test1): " << lineBuf;
            debugMsg.append(QString::number(lineno)).append(QString::fromStdString(+":" + (std::string)lineBuf));
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);   // 将当前处理的源代码行以及行号一起输出到列表文件中
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        }
        else    // fgets 失败，说明到达了文件末尾
        {
            EOF_flag = TRUE;
            qDebug() << "EOF_flag = TRUE;";
            return EOF;
        }
    }
    else return lineBuf[linepos++];     // linepos 仍小于 bufsize，说明 lineBuf 还未读完，继续读取下一个字符
    //  返回 lineBuf 数组中当前 linepos 索引处的字符(转成ASCII码对应数值)，并且 linepos 变量会在返回字符后自增
}

/* ungetNextChar backtracks one character
   in lineBuf */
/* 在 lineBuf 中回退一个字符。这个函数用于在识别到一个字符不应该被当前标记包含时，将其放回输入流中。 */
static void ungetNextChar(void)
{
    if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct
{
    const char* str;    // 保留字的字符串表示
    TokenType tok;      // 与保留字对应的标记类型
} reservedWords[MAXRESERVED]    // MAXRESERVED 在 globals.h 中定义为16
// 初始化  reservedWords 数组
= { {"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
   {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
{"write",WRITE},{"for",FOR}, {"while",WHILE}, {"endwhile",ENDWHILE}
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char* s)
{
    int i;
    for (i = 0; i < MAXRESERVED; i++)
        if (!strcmp(s, reservedWords[i].str)) // 比较输入的字符串 s 和保留字的字符串表示 str
            return reservedWords[i].tok;
    return ID;  // 如果遍历完成后没有找到匹配，返回 ID，表示输入的字符串是一个普通的标识符，而不是保留字
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{  /* index for storing into tokenString */
    int tokenStringIndex = 0;
    /* holds current token to be returned */
    TokenType currentToken;
    /* current state - always begins at START */
    StateType state = START;
    /* flag to indicate save to tokenString */
    int save;
    while (state != DONE)
    {
        int c = getNextChar();
        qDebug() << "EOF?" << c;
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
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                // 忽略空格、缩进和换行
                save = FALSE;
            else if (c == '{')
            {
                save = FALSE;
                state = INCOMMENT;      // 处于注释状态
            }
            else        // 非数字、字母、赋值以及注释
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    // ENDFILE 定义在globals.h中
                    currentToken = ENDFILE;
                    qDebug() << "ENDFILE1";
                    break;
                case '=':
                    currentToken = EQ;
                    break;
                case '>':
                    c = getNextChar(); // Read the next character
                    if (c == '=')
                    {
                        currentToken = RTEQ; // Token for >=
                    }
                    
                    else
                    {
                        ungetNextChar(); // Backup if it's not >= or <>
                        currentToken = RT; // Token for >
                    }
                    break;
                case '<':
                    c = getNextChar(); // Read the next character
                    if (c == '=')
                    {
                        currentToken = LTEQ; // Token for <=
                    }
                    else if (c == '>')
                    {
                        currentToken = NOTEQ; // Token for <>
                    }
                    else
                    {
                        ungetNextChar(); // Backup if it's not <=
                        currentToken = LT; // Token for <
                    }
                    break;
                case '+':
                    c = getNextChar(); // Read the next character
                    if (c == '+')
                    {
                        currentToken = AUTOINC; // Token for ++
                        qDebug() << "!AUTOINC!";
                    }
                    else
                    {
                        ungetNextChar(); // Backup if it's not ++
                        currentToken = PLUS; // Token for +
                    }
                    break;
                case '-':
                    c = getNextChar(); // Read the next character
                    if (c == '-')
                    {
                        currentToken = AUTODEC; // Token for --
                    }
                    else
                    {
                        ungetNextChar(); // Backup if it's not --
                        currentToken = MINUS; // Token for -
                    }
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '/':
                    currentToken = OVER;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case '[':
                    currentToken = LPM;
                    break;
                case ']':
                    currentToken = RPM;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case '%':
                    currentToken = MOD;
                    break;
                case '^':
                    currentToken = POWER;
                    break;
                case '|':
                    currentToken = OR;
                    break;
                case '&':
                    currentToken = AND;
                    break;
                case '#':
                    currentToken = CLOSE;
                    break;
                case '?':
                    currentToken = CHOOSE;
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
                qDebug() << "ENDFILE2";
            }
            else if (c == '}') state = START;
            break;
        case INASSIGN:
            state = DONE;
            if (c == '='){
                currentToken = ASSIGN;
                qDebug() << "!ASSIGN!";
            }
            else if (c == ':')
            {
                c = getNextChar(); // Read the next character
                if (c == '=')
                {
                    currentToken = REGEX; // Token for ::=
                    qDebug() << "!REGEX!";
                }
                else
                {
                    ungetNextChar();
                    ungetNextChar();    // 连续两个ungetNextChar，linepos回到了第一个:
                    save = FALSE;
                    currentToken = ERROR;   // ::除了与=能组成::=一个合法的字词，与其他字符都不能组成合法的字词
                    qDebug() << "!ERROR1!";
                }
            }
            else
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
                qDebug() << "!ERROR2!";
            }
            break;
        case INNUM:
            // 只有连续的数字字符才能作为整体存储
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
                qDebug() << "!ID!";
            }
            break;
        case DONE:
        default: /* should never happen */
            // 语法错误如::后面跟的不是=，而是其他字符
            fprintf(listing, "Scanner Bug: state= %d\n", state);
            state = DONE;
            currentToken = ERROR;
            qDebug() << "getToken default ERROR";
            break;
        }
        // MAXTOKENLEN定义在scan.h，值为40
        /*
         * tokenString数组定义在scan.h中
         * 当扫描器识别出一个标记（例如，一个标识符、数字或保留字）时，它会将这些字符逐个保存到 tokenString 数组中。
         * 这样，当一个标记被完全识别后，tokenString 数组就包含了该标记的完整文本
         */
        if ((save) && (tokenStringIndex <= MAXTOKENLEN)){
            tokenString[tokenStringIndex] = (char)c;
            qDebug() << tokenString[tokenStringIndex++];
        }

        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
//            qDebug() << tokenString;
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }
    /*TraceScan 定义在globals.h中，用于控制是否在扫描过程中输出调试信息。
     * 当 TraceScan 被设置为 TRUE 时，扫描器会在识别到每个标记后，将标记的信息输出到 listing 文件中*/
    qDebug() << "test6";
    if (TraceScan) {
        qDebug() << " test2";
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
} /* end getToken */

