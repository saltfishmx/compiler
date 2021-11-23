#include <stdarg.h>
#include <string.h>
#include "inter.h"
#define debug1  0
inode* ilist = NULL;
int labelnum = 0;
int tempnum = 0;
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
    if(kind == OCONSTANT){
        p->u.value = (int)(u);
    }
    else{
        p->u.name = (char*)(u);
        //if(debug1)printf("%s!!!\n",p->u.name);
    }
}

Operand newlabel(){
    char labelname[15]={0};
    sprintf(labelname,"label%d",labelnum++);
    int len = strlen(labelname);
    char *name = (char*)malloc(sizeof(char)*len);
    strncpy(name,labelname,len);
    Operand label = newoperand(OLABLE,name);
    return label;
}
Operand newtemp(){
    char tempname[15]={0};
    sprintf(tempname,"temp%d",tempnum++);
    int len = strlen(tempname);
    char *name = (char*)malloc(sizeof(char)*len);
    strncpy(name,tempname,len);    
    Operand temp = newoperand(OVARIABLE,name);
    if(debug1)temp->u.name = "hello";
    if(debug1)printf("%s!!~~~~\n",temp->u.name);
    return temp;
}

void genintercode(int kind,...){
    va_list valist;
    Operand op,op1,op2,result,relop,dst;
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = kind;
    inode * in = (inode*)malloc(sizeof(inode));
    in->code = ic;
    in->prev = in->next = NULL;
    switch (kind)//create several op , fullfill ic with op(s)
    {
    case ILABEL:
    case IFUNCTION:
    case IGOTO:
    case IRETURN:
    case IARG:
    case IPARAM:
    case IREAD:
    case IWRITE:  
        va_start(valist,kind);
        Operand op = va_arg(valist,Operand);
        if (op->kind == OADDRESS) {
                Operand temp = newtemp();
                genintercode(IASSIGN, temp, op);
                op = temp;
        }        
        ic->u.single.op = op;
        va_end(valist);
        addtoilist(in);
        break;

    case IASSIGN:           
    case ICALL:
    case IGETADDR:
        // after the process, all the intercode with * or & end up being broken up into some intercode + assign intercode
        va_start(valist,kind);
        Operand op1 = va_arg(valist,Operand);
        Operand op2 = va_arg(valist,Operand);
        if(op2==NULL){
            printf("error,op2 is null");
        }
        if(op1==NULL){
            printf("error,op1 is null\n kind:%d",kind);

        }    
        /*    
        if(op1->kind == OADDRESS && op2->kind ==OADDRESS){  // case like *x = *y ,should break up into two intercodes
            Operand temp = newtemp();
            genintercode(IASSIGN,temp,op2);
            op2 = temp; //temp take place of op2 then
        }
        */
        ic->u.assign.left = op1;
        ic->u.assign.right = op2;
        va_end(valist);
        addtoilist(in);
        break;

    case IADD:
    case ISUB:
    case IMUL:
    case IDIV:
        va_start(valist,kind);          
        op1 = va_arg(valist,Operand);
        op2 = va_arg(valist,Operand);
        result = va_arg(valist,Operand);     
        if(op2->kind == OADDRESS){
            Operand temp2 = newtemp();
            genintercode(IASSIGN,temp2,op2);
            op2 = temp2;          
        }
        if(op1->kind == OADDRESS){
            Operand temp1 = newtemp();
            genintercode(IASSIGN,temp1,op1);
            op1 = temp1;          
        }  
        ic->u.binop.op1 = op1;
        ic->u.binop.op2 = op2;
        if(result->kind == OADDRESS){  // *x = y + z
            Operand temp = newtemp();
            ic->u.binop.result = temp;
            addtoilist(in);
            genintercode(IASSIGN,result,temp);
        } 
        else{
            ic->u.binop.result = result;
            addtoilist(in);
        }  
        va_end(valist);
        break;
    case IDEC:
        va_start(valist,kind);
        op = va_arg(valist,Operand);
        int size = va_arg(valist,int);
        ic->u.dec.op = op;
        ic->u.dec.size = size;
        addtoilist(in);
        va_end(valist);     
        break;  
    case IIFGOTO:
        va_start(valist,kind);
        op1 = va_arg(valist,Operand);
        relop = va_arg(valist,Operand);
        op2 = va_arg(valist,Operand);
        dst = va_arg(valist,Operand);
        ic->u.ifgoto.op1 = op1; 
        ic->u.ifgoto.relop = relop;
        ic->u.ifgoto.op2 = op2;
        ic->u.ifgoto.dst = dst;
        addtoilist(in);
        va_end(valist);
        break;
    default:
        printf("something wrong happened in genintercode: unknown kind\n");
        break;
    }
    
}
void printop(FILE *f,Operand op){
    if(op->kind == OCONSTANT){
        fprintf(f,"#%d",op->u.value);
    }
    else{
        fprintf(f,"%s",op->u.name);
        if(debug1)printf("%s\n",op->u.name);
    }
}
void printic(FILE *f,InterCode ic){
    switch (ic->kind)
    {
    case IADD:
        printop(f,ic->u.binop.result);
        fprintf(f," := ");
        printop(f,ic->u.binop.op1);
        fprintf(f," + ");
        printop(f,ic->u.binop.op2);
        break;
    case ISUB:
        printop(f,ic->u.binop.result);
        fprintf(f," := ");
        printop(f,ic->u.binop.op1);
        fprintf(f," - ");
        printop(f,ic->u.binop.op2);
        break;
    case IMUL:
        printop(f,ic->u.binop.result);
        fprintf(f," := ");
        printop(f,ic->u.binop.op1);
        fprintf(f," * ");
        printop(f,ic->u.binop.op2);
        break;
    case IDIV:
        printop(f,ic->u.binop.result);
        fprintf(f," := ");
        printop(f,ic->u.binop.op1);
        fprintf(f," / ");
        printop(f,ic->u.binop.op2);
        break;  
    case IASSIGN:
        if(ic->u.assign.left->kind == OADDRESS){ // *x = y
            fprintf(f,"*");
        }
        printop(f,ic->u.assign.left);
        fprintf(f," = ");
        if(ic->u.assign.right->kind == OADDRESS){ //x = *y
            fprintf(f,"*");
        }
        printop(f,ic->u.assign.right);
        break;                          
    case IGETADDR:
        printop(f,ic->u.assign.left);
        fprintf(f," = ");
        fprintf(f,"&");
        printop(f,ic->u.assign.right);
        break; 
    case ICALL:
        printop(f,ic->u.assign.left);
        fprintf(f," = ");
        fprintf(f," CALL ");
        printop(f,ic->u.assign.right);  
        break; 
    case IARG:
        fprintf(f,"ARG ");
        printop(f,ic->u.single.op);
        break; 
    case IRETURN:
        fprintf(f,"RETURN ");
        printop(f,ic->u.single.op);
        break; 
    case IPARAM:
        fprintf(f,"PARAM ");
        printop(f,ic->u.single.op);
        break;   
    case IREAD:
        fprintf(f,"READ ");
        printop(f,ic->u.single.op);
        break;  
    case IWRITE:
        fprintf(f,"WRITE ");
        printop(f,ic->u.single.op);
        break;
    case IGOTO:
        if(debug1){
            printf("%s\n",ic->u.single.op->u.name);
            //strcpy(ic->u.single.op->u.name,"hello");
        }
        fprintf(f,"GOTO ");
        printop(f,ic->u.single.op);
        break;                    
    case ILABEL:
        fprintf(f,"LABEL ");
        printop(f,ic->u.single.op);
        fprintf(f," :");
        break;  
    case IFUNCTION:
        fprintf(f,"FUNCTION ");
        printop(f,ic->u.single.op);
        fprintf(f," :");
        break;   
    case IDEC:
        fprintf(f,"DEC ");
        printop(f,ic->u.dec.op);
        fprintf(f," %d",ic->u.dec.size);
        break;
    case IIFGOTO:
        fprintf(f,"IF ");
        printop(f,ic->u.ifgoto.op1);
        fprintf(f," ");
        printop(f,ic->u.ifgoto.relop);
        fprintf(f," ");
        printop(f,ic->u.ifgoto.op2);
        fprintf(f," GOTO ");
        printop(f,ic->u.ifgoto.dst);
        break;
    default:
        break;
    }
    fprintf(f,"\n");
}
void printintercode(FILE *f,inode *p){
    inode *head = p;
    InterCode ic;
    while(p->next!=head){ // we need one more time after while loop
        ic = p->code;
        printic(f,ic);
        p = p->next;
    }
    ic = p ->code;//now , p->next = head;
    printic(f,ic);
}