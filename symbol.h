#ifndef symbol
#define symbol
#define synboltablesize 16384
typedef struct Type_* Type;
typedef struct Symboltablenode_ Snode;
typedef struct FieldList_* FieldList;

Snode *symboltable[synboltablesize]; 
struct Type_{
    enum { BASIC, ARRAY, STRUCTURE } kind;  
    union{
        // 基本类型
        int basic; 
        // 数组类型信息包括元素类型与数组大小构成  
        struct { Type elem; int size; } array; 
        // 结构体类型信息是一个链表
        FieldList structure;
    }u;
};

struct FieldList_{
    char* name;  // 域的名字
    Type type;  // 域的类型
    FieldList tail;  // 下一个域
};

struct Symboltablenode_{
    enum{varient,function} kind;//符号表节点，符号可以是变量或者函数
    struct{
        char* name;
        Type type;
        int num;//函数参数个数
        Type *array;
    }content;
    Snode *next;
};
unsigned int hash_pjw(char* name);
#endif