#ifndef semantic
#define semantic
#include"symbol.h"
#include "tree.h"
int equals(char *s1, char *s2);
int equalType(Type1 t1, Type1 t2);
int matchproduction(Node *root, int num, ...);
Type1 translateSpecifier(Node *root);
void translateProgram(Node *root);
void translateExtDefList(Node *root);
void translateExtDef(Node *root);
void translateCompSt(Node *root,Snode *func);
void translateStmtList(Node *root,Snode *func);
void translateStmt(Node *root,Snode *func);
Snode *translateFunDec(Node *root,Type1 rettype);
void translateExtDecList(Node *root,Type1 type);
Param *translateVarlist(Node *root,int *num);
Param *translateParamDec(Node *root);
Type1 translatetype(Node *root);
Type1 translateStructspecifier(Node *root);
FieldList translateDefList(Node *root,int skind);
FieldList translateDef(Node *root,int skind);
FieldList translateDecList(Node *root,int skind ,Type1 type);
Snode *translateDec(Node *root, int skind, Type1 type);
Snode *translateVarDec(Node *root, int skind, Type1 type);
Param *translateArgs(Node *root);
Type1 translateExp(Node *root);
int checkargs(Param *p1, Param *p2, int num);
void printsemanticerror(int errornumber, int line, char *msg);
#endif