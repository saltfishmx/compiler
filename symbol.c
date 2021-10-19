#include<stdio.h>
#include"symbol.h"
#include"tree.h"
extern int *symboltable;
unsigned int hash_pjw(char* name){
    unsigned int val = 0, i;  
    for (; *name; ++name){
        val = (val << 2) + *name; 
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
    } 
    return val;
}
Type translatespecifier(Node *root){
    
}
void symbolpreorder(Node *root){
    if(root==NULL)return;
    if(strcmp(root->name,"DEF")==0){//变量

    }
    else if(){

    }
    else{
        for(int i=0;i<root->childnum;i++){
            symbolpreorder(root->childlist[i]);
        }    
    }
}