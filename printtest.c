#include"inter.h"
extern inode* ilist;
extern int labelnum;
extern int tempnum;
int main(int argc, char **argv){
    FILE *fout = fopen(argv[1], "wt+");
    Operand temp = newoperand(OVARIABLE,"main");
    Operand temp1 =newoperand(OVARIABLE,"return");
    genintercode(IFUNCTION,temp);
    genintercode(IRETURN,temp1);
    printintercode(fout,ilist);
    return 0;
}