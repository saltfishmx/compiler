#ifndef inter
#define inter 
#include <stdio.h>
#include <stdlib.h>
typedef struct Operand_ *Operand;
typedef struct inode_ inode;
typedef struct InterCode_ *InterCode;

struct Operand_
{
    enum
    {
        OVARIABLE,
        OCONSTANT,
        OADDRESS,
        OLABLE,

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
        IASSIGN,
        ICALL,
        IGETADDR,

        IADD,
        ISUB,
        IMUL,
        IDIV,

        ILABLE,
        IFUNCTION,
        IGOTO,
        IRETURN,
        IARG,
        IPARAM,
        IREAD,
        IWRITE,
        
        IDEC,
        
        IIFGOTO
     
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
        struct{
            Operand op;
        }single;
        struct{
            Operand op;
            int size;
        }dec;
        struct{
            Operand op1;
            Operand relop;
            Operand op2;
            Operand dst;
        }ifgoto;
    } u;
};
struct inode_{
    InterCode code;
    inode *prev;
    inode *next;
};
inode* ilist = NULL;
int labelnum = 0;
int tempnum = 0;
#endif