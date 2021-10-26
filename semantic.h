#ifndef semantic
#define semantic
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "symbol.h"
#include "tree.h"
int equals(char *s1, char *s2);
int equalType(Type t1, Type t2);
int matchproduction(Node *root, int num, ...);
Type translateSpecifier(Node *root);
Type translatetype(Node *root);
Type translateStructspecifier(Node *root);
FieldList translateDefList(Node *root,int skind);
FieldList translateDef(Node *root,int skind);
FieldList translateDecList(Node *root,int skind ,Type type);
Snode *translateDec(Node *root, int skind, Type type);
Snode *translateVarDec(Node *root, int skind, Type type);
Param *translateArgs(Node *root);
Type translateExp(Node *root);
int checkargs(Param *p1, Param *p2, int num);
void printsemanticerror(int errornumber, int line, char *msg);
#endif