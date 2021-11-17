#ifndef inter
#define inter 
#include <stdio.h>
#include <stdlib.h>
typedef struct Operand_ *Operand;
typedef struct inode_ inode;
typedef struct InterCode_ InterCode;

struct Operand_
{
    enum
    {
        VARIABLE,
        CONSTANT,
        ADDRESS,
        LABLE
    } kind;
    union
    {
        char *name;
        int value;
    } u;
};
struct InterCode_
{
    enum
    {
        ASSIGN,
        ADD,
        SUB,
        MUL
    } kind;
    union
    {
        struct
        {
            Operand right, left;
        } assign;
        struct
        {
            Operand result, op1, op2;
        } binop;
    } u;
};
struct inode_{
    InterCode code;
    inode *prev;
    inode *next;
};
extern inode* ilist = NULL;
extern int labelnum = 0;
extern int tempnum = 0;
#endif