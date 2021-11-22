#ifndef symbol
#define symbol
#define symboltablesize 4096
#include <stdio.h>
#include <stdlib.h>
#include"inter.h"
#include "tree.h"
typedef struct Type_* Type1;
typedef struct Symboltablenode_ Snode;
typedef struct FieldList_* FieldList;
typedef struct Param_ Param;
typedef struct funcinfo_ funcinfo;
struct Type_{
    enum { BASIC, ARRAY, STRUCTURE } kind;  
    int rvalue;
    union{
        // 基本类型
        enum{INT1,FLOAT1} basic; 
        // 数组类型信息包括元素类型与数组大小构成  
        struct { Type1 elem; int size; } array; 
        // 结构体类型信息是一个链表
        struct{char *strname;FieldList fild;}structure;
    }u;
};

struct FieldList_{
    char* name;  // 域的名字
    Type1 type;  // 域的类型
    FieldList tail;  // 下一个域
};
struct Param_{
    Type1 type;
    Operand op;//lab3
    Param* tail;
};
struct funcinfo_{
    Type1 rettype;
    Param *p;
    int paramnum;
};
struct Symboltablenode_{
    enum{varient,function,stru,field} kind;//符号表节点，符号可以是变量或者函数 结构体的定义 or field
    char* name;
    union{
        Type1 type;
        funcinfo* f; //func
    }content;
    Snode *next;
};
extern Snode *symboltable[];

unsigned int hash_pjw(char* name);
void inithash(Snode **table);
void linknodeadd(Snode **table, int pos, Snode *a);
Snode *createsnode(char *name, Type1 t, int skind);
void addtosymboltable(Snode *n);
Snode* contain(char *name, int typewanted);
#endif