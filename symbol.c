#include<string.h>
#include "semantic.h"
#include "symbol.h"
extern Snode *symboltable[];
unsigned int hash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0xfff)
            val = (val ^ (i >> 12)) & 0xfff;
    }
    return val;
}
void inithash(Snode **table)
{
    table = (Snode**)malloc(sizeof(Snode*)*symboltablesize);
    for(int i=0;i<symboltablesize;i++){
        table[i] = NULL;
    }
    //lab3:在符号表中预先添加read和write这两个预定义的函数
    Type1 t = (Type1)malloc(sizeof(struct Type_));
    t->kind = BASIC;
    t->u.basic = INT1;
    t->rvalue = 1;
    Snode *n = createsnode("read",t,function);
    n->content.f->paramnum = 0;
    n->content.f->p = NULL;
    addtosymboltable(n);
    
    Type1 t1 = (Type1)malloc(sizeof(struct Type_));
    t1->kind = BASIC;
    t1->u.basic = INT1;
    t1->rvalue = 1;    
    Snode *n1 =  createsnode("write",t1,function);
    n1->content.f->paramnum = 1;
    Type1 t2 = (Type1)malloc(sizeof(struct Type_));
    t2->kind = BASIC;
    t2->u.basic = INT1;
    t2->rvalue = 1;        
    Param *p = (Param*)(malloc)(sizeof(struct Param_));
    p->type = t2;
    p->tail = NULL;
    n1->content.f->p = p;
    addtosymboltable(n1);
}
void linknodeadd(Snode **table, int pos, Snode *a)
{ //add a snode to symboltable

    if (table[pos] == NULL)
    { //will this work after memset?
        table[pos] = a;
    }
    else
    {
        Snode *t = (table[pos]);
        while (t->next != NULL)
        {
            t = t->next;
        }
        t->next = a;
    }
}
Snode *createsnode(char *name, Type1 t, int skind)
{ //create a snode , in realtion to func kind , more information should be fulfilled

    Snode *n = malloc(sizeof(Snode));
    n->next = NULL;
    n->kind = skind;
    switch (skind)
    {
    case varient:
    case field:
    case stru:
        n->name = name;
        n->content.type = t;
        break;
    case function:
        n->name = name;
        (n->content).f->rettype = t;
        break;
    default:
        printf("error unknown snode type \n");
        break;
    }
    return n;
}
void addtosymboltable(Snode *n)
{ //add to symboltable
    int pos = hash_pjw(n->name);
    linknodeadd(symboltable, pos, n);
}
Snode* contain(char *name, int typewanted)
{ //tip : a varient's name cant be the same with kind's name including a user-defined kind(such as structure)
    //tyoe:varient,function,stru
    int pos = hash_pjw(name);
    //printf("%d",pos);
    if (symboltable[pos]==NULL)
    {
       
        //fprintf(stderr,"????");
        return NULL;
    }
    else
    {
        //fprintf(stderr,"??");
        Snode *n = symboltable[pos];
        
        while (n != NULL && !equals(name, n->name))
        {
            
            n = n->next;
        }
        if(n==NULL){
            return NULL;
        }
        else{
            if(n->kind!=typewanted){ // for example .you want to check a varient named node but it turns out to be the name of some structure
                return NULL;
            }
            return n;
        }
    }
}
