#include <stdio.h>
#include "syntax.tab.h"
#include "tree.h"
#include"symbol.h"
#include"semantic.h"
#include "inter.h"
extern int yyparse();
extern void yyrestart(FILE*);
extern Snode *symboltable[];
extern inode* ilist;
int error = 0;
int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "wt+");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    if (!fout)
    {
        perror(argv[2]);
        return 1;
    }    
    yyrestart(f);
    yyparse();
    if(!error){
        inithash(symboltable);
        translateProgram(root);
        printintercode(fout,ilist);
        //preorder(root,0);
    }
    return 0;
}
