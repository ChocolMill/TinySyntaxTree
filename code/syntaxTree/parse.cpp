#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include <string>
#include <QDebug>
using namespace std;
#pragma execution_character_set("utf-8")

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode* stmt_sequence(void);
static TreeNode* statement(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
static TreeNode* exp(void);
static TreeNode* simple_exp(void);
static TreeNode* term(void);
static TreeNode* power(void);
static TreeNode* autofactor(void);
static TreeNode* factor(void);
static TreeNode* for_stmt(void);
static TreeNode* while_stmt(void);
static TreeNode* regex_exp(void);
static TreeNode* andreg(void);
static TreeNode* topreg(void);
static TreeNode* reg_factor(void);


static void syntaxError(string message)
{
    qDebug() << "error in line " << lineno << " :" << QString::fromStdString(message);
    string msg = ": " + message;
    debugMsg.append("\nerror in line ").append(QString::number(lineno)).append(QString::fromStdString(msg));
    /*fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);*/
    Error = TRUE;
}

static void match(TokenType expected)
{
    if (token == expected) token = getToken();
    else {
        qDebug() << "ERROR match!";
        syntaxError("unexpected token -> " + getTokenString(token, tokenString).toStdString());
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

TreeNode* stmt_sequence(void)
{
    TreeNode* t = statement();
    TreeNode* p = t;
    qDebug() << "stmt_sequence: " << token;
    while ((token != ENDFILE) && (token != END) &&
        (token != ELSE) && (token != UNTIL) && (token != RPM) && (token != ENDWHILE))
    {
        TreeNode* q;
        match(SEMI);
        q = statement();
        if (q != NULL) {
            // t always points to the first node of the statemnet linklist.
            if (t == NULL) t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q; // pq一前一后在语句链表上移动，p q move on the statement linklist.
                p = q;
            }
        }
    }
    return t;
}


//P394
//lineno: 961
TreeNode* statement(void)
{
    TreeNode* t = NULL;
    switch (token) {
    case IF: t = if_stmt(); break;
    case REPEAT: t = repeat_stmt(); break;
    case ID: t = assign_stmt(); break;
    case READ: t = read_stmt(); break;
    case WRITE: t = write_stmt(); break;
    case FOR: t = for_stmt(); break;
    case WHILE: t = while_stmt(); break;
    case AUTOINC:
    case AUTODEC: t = autofactor(); break;
    default: syntaxError("unexpected token -> " + getTokenString(token, tokenString).toStdString());
        printToken(token, tokenString);
        token = getToken();
        break;
    } /* end case */
    return t;
}


//P394
//lineno: 977
TreeNode* if_stmt(void)
{
    TreeNode* t = newStmtNode(IfK);
    match(IF);
    match(LPAREN);
    if (t != NULL) t->child[0] = exp();
    match(RPAREN);
    match(LPM);
    if (t != NULL) t->child[1] = stmt_sequence();
    match(RPM);
    if (token == ELSE) {
        match(ELSE);
        match(LPM);
        if (t != NULL) t->child[2] = stmt_sequence();
        match(RPM);
    }
    return t;
}

//P394
//lineno:991
TreeNode* repeat_stmt(void)
{
    TreeNode* t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t != NULL) t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t != NULL) t->child[1] = exp();
    return t;
}

TreeNode* assign_stmt(void)
{
    TreeNode* t = newStmtNode(AssignK);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString); // tokenString;
    match(ID);
    if (token == ASSIGN)
    {
        match(ASSIGN);
        if (t != NULL) t->child[0] = exp();
    }
    else if (token == REGEX)
    {
        qDebug() << "assign_stmt: " << token;
        match(REGEX);
        if (t != NULL) t->child[0] = regex_exp();
    }
    return t;
}

TreeNode* read_stmt(void)
{
    TreeNode* t = newStmtNode(ReadK);
    match(READ);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    return t;
}

TreeNode* write_stmt(void)
{
    TreeNode* t = newStmtNode(WriteK);
    match(WRITE);
    if (t != NULL) t->child[0] = exp();
    return t;
}


TreeNode* exp(void)
{
    TreeNode* t = simple_exp();
    if (token == LTEQ || token == RTEQ || token == LT || token == RT || token == NOTEQ || token == EQ) {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t != NULL)
            t->child[1] = simple_exp();
    }
    return t;
}

TreeNode* simple_exp(void)
{
    TreeNode* t = term();
    while ((token == PLUS) || (token == MINUS))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode* term(void)
{
    TreeNode* t = power();
    while ((token == TIMES) || (token == OVER) || (token == MOD))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = power();
        }
    }
    return t;
}

TreeNode* power(void)
{
    TreeNode* t = autofactor();
    while ((token == POWER))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = autofactor();
        }
    }
    return t;
}

TreeNode* autofactor(void){
//    qDebug() << "autofactor: " << token;
    TreeNode* t = nullptr;
    if(token == AUTOINC || token == AUTODEC){
//        a:=++b
//        match(token);     AUTO -> ID
//        t = factor();     ID -> EOF
//        TreeNode* p = newExpNode(OpK);
//        p->child[0] = t;
//        p->attr.op = token;
//        t = p;
        TreeNode* p = newExpNode(OpK);
        p->attr.op = token;
        match(token);
        t = factor();
        p->child[0] = t;
        t = p;
    }
    else{
        t = factor();
    }
    return t;
}

TreeNode* factor(void)
{
    TreeNode* t = NULL;
    switch (token) {
    case NUM:
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        qDebug() << "factor case NUM: " << token;
        break;
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        qDebug() << "factor case ID: " << token;
        break;
    case LPAREN:
        match(LPAREN);
        t = exp();
        match(RPAREN);
        break;
    default:
        qDebug() << "test4";
        syntaxError("unexpected token -> " + getTokenString(token, tokenString).toStdString());
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}


TreeNode* for_stmt(void)
{
    match(FOR);
    match(LPAREN);
    TreeNode* p = assign_stmt();
    TreeNode* t = NULL;
    t = newStmtNode(ForK);
    t->child[0] = p;
    match(SEMI);
    t->child[1] = exp();
    match(SEMI);
    t->child[2] = autofactor();
    match(RPAREN);
    match(LPM);
    t->child[3] = stmt_sequence();
    match(RPM);
    return t;
}

TreeNode* while_stmt(void){
    TreeNode* t = NULL;
    t = newStmtNode(WhileK);
    match(WHILE);
    match(LPAREN);
    t->child[0] = exp();
    match(RPAREN);
    t->child[1] = stmt_sequence();
    match(ENDWHILE);
    return t;
}

TreeNode* regex_exp(void)
{
    TreeNode* t = andreg();
    while ((token == OR))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = andreg();
        }
    }
    return t;
}

TreeNode* andreg(void)
{
    TreeNode* t = topreg();
    while ((token == AND))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = topreg();
        }
    }
    return t;
}

TreeNode* topreg(void)
{
    TreeNode* t = reg_factor();
    while ((token == CLOSE) || (token == CHOOSE))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
        }
    }
    return t;
}

TreeNode* reg_factor(void)
{
    TreeNode* t = NULL;
    switch (token) {
    case NUM:
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN:
        match(LPAREN);
        t = regex_exp();
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> " + getTokenString(token, tokenString).toStdString());
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode* parse(void)
{
    TreeNode* t;
    token = getToken();
    qDebug() << "parse: " << token;
    t = stmt_sequence();
    if (token != ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}
