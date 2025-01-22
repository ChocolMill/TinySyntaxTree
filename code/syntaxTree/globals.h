#pragma once
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <QString>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 11

typedef enum
/* book-keeping tokens */
{
    ENDFILE, ERROR,
    /* reserved words */
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE, FOR, WHILE, ENDWHILE,
    /* multicharacter tokens */
    ID, NUM,
    /* special symbols */
    ASSIGN, EQ, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI,
    /* �˷���ģ */
    POWER, MOD,
    /* С�ڣ����ڣ�С�ڵ��ڣ����ڵ��ڣ������� */
    LT, RT, LTEQ, RTEQ, NOTEQ,
    /* ǰ��������ǰ���Լ� */
    AUTOINC, AUTODEC,
    REGEX,OR,AND,CLOSE,CHOOSE,
    /* �������ţ��������� */
    LPM,RPM
} TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */   // ������һ��ȫ�ֱ��������ڸ���Դ������кš�

extern QString debugMsg;

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, RepeatK, AssignK, ReadK, WriteK, ForToK, ForDowntoK, ForK, WhileK, RegexK} StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 4

typedef struct treeNode
{
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;
    int lineno;
    NodeKind nodekind;
    // union ���ص��������г�Ա����ͬһ���ڴ�ռ䣬����ζ�����κ�ʱ��ֻ��ʹ������һ����Ա����Ϊ�������ڴ����ص�
    union { StmtKind stmt; ExpKind exp; } kind; // ȡ���� TreeNode ��ʾ�������ڵ㻹�Ǳ��ʽ�ڵ�
    union {
        TokenType op;   // ������
        int val;        // ����ֵ
        char* name;     // ������
    } attr;     // ȡ���ڽڵ�ľ�������
    ExpType type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif
