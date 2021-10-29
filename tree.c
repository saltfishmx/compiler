#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include"tree.h"
#include"syntax.tab.h"
Node* malloc_node(char *name,int type,void* val,int line){
    Node *node = malloc(sizeof(Node));
    node->name = malloc(strlen(name)+1);
    strcpy(node->name,name);
    node->childnum = 0;
    node->type = type;
    node->lineno = line;

    for(int i=0;i<10;i++){
        node->childlist[i]=NULL;
    }
    if(type==INT){
        node->val.i = *(int*)val;
    }
    else if(type == FLOAT){
        node->val.f = *(float*)val;
    }
    else if(type==ID||type==Type){
        node->val.s = malloc(strlen(val)+1);
        strcpy(node->val.s,val);
    }
    else{
        node->val.i = 0;
    }
    return node;
}
void *add(Node *father,Node *son){
    father->childlist[father->childnum++]=son;
}
void print_space(int n){
    for(int i=0;i<2*n;i++){
        printf(" ");
    }
}
void preorder(Node *root,int space_num){
    if(root==NULL)return;
    print_space(space_num);
    switch (root->type)
    {
    case 0://gramma unit not generating empty 
        /* code */
        printf("%s (%d)\n", root->name, root->lineno);
        break;
    case INT:
        printf("%s: %d\n", root->name, root->val.i);
        break;
    case FLOAT:
        printf("%s: %f\n", root->name, root->val.f);
        break;
    case Type:
    case ID:
        printf("%s: %s\n", root->name, root->val.s);
        break;
    default:
        printf("%s\n", root->name);
        break;
    }
    space_num++;
    for(int i=0;i<root->childnum;i++){
        preorder(root->childlist[i],space_num);
    }
    space_num--;
}