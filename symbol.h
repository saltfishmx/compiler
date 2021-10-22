#ifndef symbol
#define symbol
#define symboltablesize 16384
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include"semantic.h"
typedef struct Type_* Type;
typedef struct Symboltablenode_ Snode;
typedef struct FieldList_* FieldList;
typedef struct Param_ Param;

Snode *symboltable[symboltablesize]; 
struct Type_{
    enum { BASIC, ARRAY, STRUCTURE } kind;  
    union{
        // 基本类型
        enum{INT,FLOAT} basic; 
        // 数组类型信息包括元素类型与数组大小构成  
        struct { Type elem; int size; } array; 
        // 结构体类型信息是一个链表
        struct{char *strname;FieldList fild}structure;
    }u;
};

struct FieldList_{
    char* name;  // 域的名字
    Type type;  // 域的类型
    FieldList tail;  // 下一个域
};

struct Symboltablenode_{
    enum{varient,function,stru} kind;//符号表节点，符号可以是变量或者函数 结构体的定义
    char* name;
    union{
        Type type;
        Param p; //func
    }content;
    Snode *next;
};
struct Param_{
    int num;
    Type rettype;
    Type *array;
};
unsigned int hash_pjw(char* name);
void inithash(Snode *table);
void linknodeadd(Snode *table, int pos, Snode *a);
Snode *createsnode(char *name, Type t, int skind);
void addtosymboltable(Snode *n);
int contain(char *name, int typewanted);
void symbolpreorder(Node *root);
#endif