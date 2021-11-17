#include "inter.h"
extern inode* ilist;
extern int labelnum;
extern int tempnum;
void addtoilist(inode* n){
    if(ilist == NULL){
        ilist = n;
        n->next = n->prev = n;
    }
    else{ // add to tail
        inode *presenttail = ilist ->prev;
        presenttail -> next = n;
        n->prev = presenttail;
        n->next = ilist;
        ilist->prev = n;
    }
}
Operand newoperand(int kind,void *u){
    Operand p = (Operand)malloc(sizeof(struct Operand_));
    p->kind = kind;
    if(kind == CONSTANT){
        p->u.value = (int)u;
    }
    else{
        p->u.value = (char*)u;
    }
}
Operand newlabel(){
    char labelname[10];
    sprintf(labelname,"label%d",labelnum++);
    Operand label = newoperand(LABLE,labelname);
    return label;
}
Operand newtemp(){
    char tempname[10];
    sprintf(tempname,"label%d",tempnum++);
    Operand temp = newoperand(VARIABLE,tempname);
    return temp;
}