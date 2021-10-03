#include <stdio.h>
#include "syntax.tab.h"
#include "tree.h"
extern int yyparse();
extern void yyrestart(FILE*);
int error = 0;
int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if(!error){
        preorder(root,0);
    }
    return 0;
}
