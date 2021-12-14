#include <stdio.h>
#include<string.h>
#include "assembly.h"
#include "inter.h"
// 8 : t0
// $25($t9) keeps constant
#define reginum 25
#define maxvarinum 100
inode *head;
char *regi[reginum];
varipair vari;
int lastuse = 0;
int offset = 0;
void initregi()
{
    vari = (varipair)malloc(sizeof(struct varipair_) * maxvarinum);
    for (int i = 0; i < reginum; i++)
    {
        regi[i] = NULL;
        vari[i].s = NULL;
        vari[i].offset = 0;
    }
}
void clearreg(){
    for(int i=8;i<reginum;i++){
        regi[i] = NULL;
    }
}
int reg(char *variname, FILE *f)
{

    for (int i = 8; i < reginum; i++)
    {

        if (regi[i] == NULL)
        { // found free register
            regi[i] = news(variname);
            lastuse = i;

            return i;
        }
        else{
            if(regi[i]==NULL){
                printf("regi[i]==NULL!\n");
            }
            if(variname==NULL){
                printf("variname==NULL!\n");
            }   
            //printf("variname %s 123\n",variname);
            //printf("reg i %s 123\n",regi[i]);         
            //printf("strcmp result %d\n", strcmp(variname, regi[i]));
            if (strcmp(variname, regi[i]) == 0)
            { // already in register
                
                lastuse = i;
                return i;
            }
        }

    }
    int firstblank = -1;
    //reach here means not in regi ,and no allocatable register, needs to choose a allocated register
    for (int i = 8; i < reginum; i++)
    {
        if (i != lastuse)
        { // i : the register number to be reallocated
            fprintf(f, "subu $sp, $sp , 4\n");
            fprintf(f, "sw $%d,%d($s8)\n", i, offset);
            offset -= 4;
            for (int j = 0; j < maxvarinum; j++)
            {
                if (vari[j].s != NULL && strcmp(vari[j].s, variname) == 0)
                { //already in stack
                    fprintf(f, "lw $%d,%d($s8)\n", i, vari[j].offset);
                    //free(vari[j].s);
                    vari[j].s = NULL;
                    vari[j].offset = 0;
                    lastuse = i;
                    return i;
                }
                else if (vari[j].s == NULL && firstblank == -1)
                {
                    firstblank = j;
                }
            }
            if (firstblank != -1)
            {
                vari[firstblank].s = (regi[i]);
                vari[firstblank].offset = offset;
            }
            lastuse = i;
            return i;            
        }
    }
    printf("error!!!");
    return -1;
}

void printassembly(FILE *f, inode *p)
{
    InterCode ic = p->code;
    InterCode ic1;
    int a,b;
    char *dst;
    int argnum = 0;
    int count = 0;
    inode* p1;
    switch (ic->kind)
    {
    case IADD:
        if (ic->u.binop.op1->kind == OCONSTANT && ic->u.binop.op2->kind == OCONSTANT)
        {
            fprintf(f, "  li $%d, %d", reg(ic->u.binop.result->u.name, f), ic->u.binop.op1->u.value + ic->u.binop.op2->u.value);
        }
        else if (ic->u.binop.op1->kind == OCONSTANT || ic->u.binop.op2->kind == OCONSTANT)
        {
            if (ic->u.binop.op1->kind == OCONSTANT)
            {
                fprintf(f, "  addi $%d, $%d, %d", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op2->u.name, f), ic->u.binop.op1->u.value);
            }
            else
            {
                fprintf(f, "  addi $%d, $%d, %d", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op1->u.name, f), ic->u.binop.op2->u.value);
            }
        }
        else
        {
            fprintf(f, "  add $%d, $%d, $%d", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op1->u.name, f), reg(ic->u.binop.op2->u.name, f));
        }

        break;
    case ISUB:
        if (ic->u.binop.op1->kind == OCONSTANT && ic->u.binop.op2->kind == OCONSTANT)
        {
            fprintf(f, "  li $%d, %d", reg(ic->u.binop.result->u.name, f), ic->u.binop.op1->u.value - ic->u.binop.op2->u.value);
        }
        else if (ic->u.binop.op1->kind == OCONSTANT || ic->u.binop.op2->kind == OCONSTANT)
        {
            if (ic->u.binop.op1->kind == OCONSTANT)
            {
                printf("error happened!");
            }
            else
            {
                fprintf(f, "  addi $%d, $%d, %d", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op1->u.name, f), (-1) * ic->u.binop.op2->u.value);
            }
        }
        else
        {
            fprintf(f, "  sub $%d, $%d, $%d", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op1->u.name, f), reg(ic->u.binop.op2->u.name, f));
        }
        break;
    case IMUL:
        if (ic->u.binop.op1->kind == OCONSTANT && ic->u.binop.op2->kind == OCONSTANT)
        {
            fprintf(f, "  li $%d, %d", reg(ic->u.binop.result->u.name, f), ic->u.binop.op1->u.value * ic->u.binop.op2->u.value);
        }
        else if (ic->u.binop.op1->kind == OCONSTANT)
        {
            fprintf(f, "  li $25, %d\n", ic->u.binop.op1->u.value);
            fprintf(f, "  mul $%d, $%d, $25", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op2->u.name, f));
        }
        else if (ic->u.binop.op2->kind == OCONSTANT)
        {
            fprintf(f, "  li $25, %d\n", ic->u.binop.op2->u.value);
            fprintf(f, "  mul $%d, $%d, $25", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op1->u.name, f));
        }
        else
        {
            fprintf(f, "  mul $%d, $%d, $%d", reg(ic->u.binop.result->u.name, f), reg(ic->u.binop.op1->u.name, f), reg(ic->u.binop.op2->u.name, f));
        }
        break;
    case IDIV:
        if (ic->u.binop.op1->kind == OCONSTANT && ic->u.binop.op2->kind == OCONSTANT)
        {
            fprintf(f, "  li $%d, %d", reg(ic->u.binop.result->u.name, f), ic->u.binop.op1->u.value / ic->u.binop.op2->u.value);
        }
        else if (ic->u.binop.op1->kind == OCONSTANT)
        {
            fprintf(f, "  li $25, %d\n", ic->u.binop.op1->u.value);
            fprintf(f, "  div $25, $%d\n", reg(ic->u.binop.op2->u.name, f));
            fprintf(f, "  mflo $%d", reg(ic->u.binop.result->u.name, f));
        }
        else if (ic->u.binop.op2->kind == OCONSTANT)
        {
            fprintf(f, "  li $25, %d\n", ic->u.binop.op2->u.value);
            fprintf(f, "  div $%d, $25\n", reg(ic->u.binop.op1->u.name, f));
            fprintf(f, "  mflo $%d", reg(ic->u.binop.result->u.name, f));
        }
        else
        {
            fprintf(f, "  div $%d, $%d\n", reg(ic->u.binop.op1->u.name, f), reg(ic->u.binop.op2->u.name, f));
            fprintf(f, "  mflo $%d", reg(ic->u.binop.result->u.name, f));
        }
        break;
    case IASSIGN: //指令用法为：sw rt，offset（base）。
                  //指令作用为：将地址为rt的通用寄存器的值存储到内存中的指定地址。该指令有地址对齐要求，要求计算出来的存储地址的最低两位为00。
        if (ic->u.assign.left->kind == OADDRESS)
        { // *x = y
            fprintf(f, "  sw $%d, 0($%d)", reg(ic->u.assign.right->u.name, f), reg(ic->u.assign.left->u.name, f));
        }
        else if (ic->u.assign.right->kind == OADDRESS)
        { //x = *y
            fprintf(f, "  lw $%d, 0($%d)", reg(ic->u.assign.left->u.name, f), reg(ic->u.assign.right->u.name, f));
        }
        else if (ic->u.assign.right->kind == OCONSTANT)
        { //x := #k
            fprintf(f, "  li $%d, %d", reg(ic->u.assign.left->u.name, f), ic->u.assign.right->u.value);
        }
        else
        { // x := y
            fprintf(f, "  move $%d, $%d", reg(ic->u.assign.left->u.name, f), reg(ic->u.assign.right->u.name, f));
        }
        break;
    case IGETADDR: //la Rdest, addr
        fprintf(f, "  la $%d, %s", reg(ic->u.assign.left->u.name, f), ic->u.assign.right->u.name);
        break;
    case ICALL:
        fprintf(f, "  addi $sp, $sp, -4\n");
        fprintf(f, "  sw $ra, 0($sp)\n");
        count = pusha(f);
        p1=p->prev;
        ic1 = p1->code;
        while(ic1->kind == IARG){
            //printf("argnum:%d",argnum);
            argnum++;
            p1 = p1->prev;
            ic1 = p1->code;
        }
        fprintf(f,"  addi $sp, $sp, -%d\n",4 * (argnum>4?4:argnum));    
        for(int i=0;i<4&&i<argnum;i++){
            fprintf(f,"  sw $a%d, %d($sp)\n",i,i*4);
        }

        argnum = 0;
        p1 = p->prev;
        ic1 = p1->code;
        while(ic1->kind == IARG){//ARG z、ARG y、ARG x和t1 := CALL func
            
            if(argnum<4){
                fprintf(f,"  move $a%d, $%d\n",argnum,reg(ic1->u.single.op->u.name,f));
            }
            else{
                fprintf(f, "  addi $sp, $sp, -4\n");
                fprintf(f, "  sw $%d, 0($sp)\n",reg(ic1->u.single.op->u.name,f));
                fprintf(f, "  move $fp, $sp\n");
            }
            argnum++;
            p1 = p1->prev;
            ic1 = p1->code;
        }

        fprintf(f, "  jal %s\n", ic->u.assign.right->u.name);
       
        
        for(int i=0;i<4&&i<argnum;i++){
            fprintf(f,"  lw $a%d, %d($sp)\n",i,i*4);
        }          
        fprintf(f,"  addi $sp, $sp, %d\n",(argnum>4?4:argnum)*4);       
        popa(f,count);

        fprintf(f, "  lw $ra, 0($sp)\n");    
        fprintf(f, "  addi $sp, $sp, 4\n");    
        fprintf(f, "  move $%d, $v0", reg(ic->u.assign.left->u.name, f));
        break;

    case IRETURN:
        if(ic->u.single.op->kind ==  OCONSTANT){
            fprintf(f, "  li $25, %d\n",ic->u.single.op->u.value);
            fprintf(f, "  move $v0, $25\n");
            
        }
        else{
            fprintf(f, "  move $v0, $%d\n", reg(ic->u.single.op->u.name, f));
        }
        
        fprintf(f, "  jr $ra");
        break;

    case IARG:
    //ICALL process
        return;
        break;
    case IPARAM:
    //IFUNCTION process
        return;
        break;

    case IGOTO:
        fprintf(f, "  j %s", ic->u.single.op->u.name);
        break;

    case ILABEL:
        fprintf(f, "%s:", ic->u.single.op->u.name);
        break;
    case IFUNCTION:
        clearreg();
        fprintf(f, "\n%s:", ic->u.single.op->u.name);
        p1 = p->next;
        ic1 = p1->code;
        while(ic1->kind == IPARAM){
            
            if(argnum<4){
                fprintf(f, "  move $%d, $a%d",reg(ic1->u.single.op->u.name,f),argnum);
            }
            else{
                fprintf(f, "  lw $%d, %d($fp)",reg(ic1->u.single.op->u.name,f),4*(argnum-4));
            }
            argnum++;
            p1 = p1->next;
            ic1 = p1->code;
        }
        break;

    case IIFGOTO:
        a = reg(ic->u.ifgoto.op1->u.name, f);
        if(ic->u.ifgoto.op2->kind==OCONSTANT){
            fprintf(f,"  li $25, %d\n",ic->u.ifgoto.op2->u.value);
            b = 25;
        }
        else{
            b = reg(ic->u.ifgoto.op2->u.name, f);
        }
        
        dst = ic->u.ifgoto.dst->u.name;
        if (strcmp(ic->u.ifgoto.relop->u.name, "==") == 0)
        {
            fprintf(f,"  beq $%d, $%d, %s", a, b, dst);
        }
        else if (strcmp(ic->u.ifgoto.relop->u.name, "!=") == 0)
        {
            fprintf(f,"  bne $%d, $%d, %s", a, b, dst);
        }
        else if (strcmp(ic->u.ifgoto.relop->u.name, ">") == 0)
        {
            fprintf(f,"  bgt $%d, $%d, %s", a, b, dst);
        }
        else if (strcmp(ic->u.ifgoto.relop->u.name, "<") == 0)
        {
            fprintf(f,"  blt $%d, $%d, %s", a, b, dst);
        }
        else if (strcmp(ic->u.ifgoto.relop->u.name, ">=") == 0)
        {
            fprintf(f,"  bge $%d, $%d, %s", a, b, dst);
        }
        else if (strcmp(ic->u.ifgoto.relop->u.name, "<=") == 0)
        {
            fprintf(f,"  ble $%d, $%d, %s", a, b, dst);
        }
        break;
    case IREAD:
        fprintf(f,"  addi $sp, $sp, -4\n");
        fprintf(f,"  sw $ra, 0($sp)\n");
        fprintf(f,"  jal read\n");
        fprintf(f,"  lw $ra, 0($sp)\n");
        fprintf(f,"  addi $sp, $sp, 4\n");
        fprintf(f,"  move $%d, $v0",reg(ic->u.single.op->u.name,f));
        break;
    case IWRITE:
        
        fprintf(f,"  addi $sp, $sp, -8\n");
        fprintf(f,"  sw $ra, 0($sp)\n");
        fprintf(f,"  sw $a0, 4($sp)\n"); //a0 may store other things inside
        fprintf(f,"  move $a0, $%d \n",reg(ic->u.single.op->u.name,f));
        fprintf(f,"  jal write\n");
        fprintf(f,"  lw $ra, 0($sp)\n");
        fprintf(f,"  lw $a0, 4($sp)\n");
        fprintf(f,"  addi $sp, $sp, 8\n");  
        break;
    default:
        return;
        break;
    }
    fprintf(f, "\n");
}
int pusha(FILE *f){
    // $8 - $24 ,17 in total
    int count = 0;
    for(int i = 8;i<25;i++){
        if(regi[i]==NULL){
            count = i - 8;
            break;
        }
    }
   
    if(count==0)return 0;
    fprintf(f,"  addi $sp, $sp, -%d\n",count*4);
    for(int i=8;i<count+8;i++){
        fprintf(f,"  sw $%d, %d($sp)\n",i,(i-8)*4);
    }

    return count;
}
void popa(FILE *f,int count){
    
    for(int i=8;i<count + 8;i++){
        fprintf(f,"  lw $%d, %d($sp)\n",i,(i-8)*4);
    }
    fprintf(f,"  addi $sp, $sp, %d\n",count*4);   

}
void printobjectcode(FILE *f, inode *p)
{
    initregi();
    fprintf(f, ".data\n");
    fprintf(f, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, ".globl main\n");
    head = p;
    InterCode ic;
    while (p->next != head)
    { // we need one more time after while loop
        ic = p->code;
        if (ic->kind == IDEC)
        {
            fprintf(f, "%s: .word %d\n", ic->u.dec.op->u.name, ic->u.dec.size);
        }
        p = p->next;
    }
    ic = p->code; //now , p->next = head;
    if (ic->kind == IDEC)
    {
        fprintf(f, "%s: .word %d\n", ic->u.dec.op->u.name, ic->u.dec.size);
    }
    fprintf(f, ".text\n");
    fprintf(f, "read:\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _prompt\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 5\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  jr $ra\n\n");

    fprintf(f, "write:\n");
    fprintf(f, "  li $v0, 1\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _ret\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  move $v0, $0\n");
    fprintf(f, "  jr $ra\n");
    p = head;
    while (p->next != head)
    { // we need one more time after while loop
        printassembly(f, p);
        p = p->next;
    }
    //now , p->next = head;
    printassembly(f, p);
}
