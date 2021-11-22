#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "semantic.h"
#include "symbol.h"
#include "inter.h"
#include "syntax.tab.h"
#define debug 0
int anonymousnum = 0;
extern Snode *symboltable[];

int getsize(Type1 type)
{
    if (type == NULL)
    {
        return 0;
    }
    else if (type->kind == BASIC)
    {
        return 4;
    }
    else if (type->kind == ARRAY)
    {
        return getsize(type->u.array.elem) * (type->u.array.size);
    }
    else if (type->kind == STRUCTURE)
    {
        int num = 0;
        FieldList f = type->u.structure.fild;
        while (f != NULL)
        {
            num += getsize(f->type);
            f = f->tail;
        }
        return num;
    }
}
int equals(char *s1, char *s2)
{
    if (strcmp(s1, s2) == 0)
    {
        return 1;
    }
    else
        return 0;
}
int equalType(Type1 t1, Type1 t2)
{
    if (t1 == NULL || t2 == NULL)
    {
        return 0;
    }
    if (t1->kind != t2->kind)
    {
        return 0;
    }
    if (t1->kind == BASIC)
    {
        return t1->u.basic == t2->u.basic;
    }
    else if (t1->kind == ARRAY)
    { //只要数组的基类型和维数相同我们即认为类型是匹配的
        return equalType(t1->u.array.elem, t2->u.array.elem);
    }
    else if (t1->kind == STRUCTURE) //结构等价
    {
        FieldList f1 = t1->u.structure.fild;
        FieldList f2 = t2->u.structure.fild;
        while (f1 != NULL && f2 != NULL)
        {
            if (!equalType(f1->type, f2->type))
            {
                return 0;
            }
            f1 = f1->tail;
            f2 = f2->tail;
        }
        if (f1 != NULL || f2 != NULL)
            return 0;
        else
            return 1;
    }
}
int matchproduction(Node *root, int num, ...)
{

    va_list valist;
    va_start(valist, num);
    int res = 1;
    for (int i = 0; i < num; i++)
    {

        char *name = va_arg(valist, char *);
        /*
        if(equals(name,"Stmt")){
            printf("qwirhiqhwriqhwir\n");
            //printf("%s\n",root->childlist[1]->name); 
            printf("%s\n",root->childlist[0]->name);    
        }        
        */
        if (equals(name, "NULL") && (root->childlist[i] == NULL))
        {
            continue;
        }
        if (root->childlist[i] == NULL)
        {
            res = 0;
            break;
        }

        if (!equals(name, root->childlist[i]->name))
        {
            res = 0;
            break;
        }
    }
    va_end(valist);

    if (root->childlist[num] != NULL)
    {
        res = 0;
    }
    return res;
}
Type1 translateSpecifier(Node *root)
{
    if (debug)
    {
        printf("translateSpecifier,lineo:%d\n", root->lineno);
    }
    if (matchproduction(root, 1, "TYPE"))
    {
        return translatetype(root->childlist[0]);
    }
    else if (matchproduction(root, 1, "StructSpecifier"))
    {
        return translateStructspecifier(root->childlist[0]);
    }
    else
    {
        //printf("root->childlist[0]: %s   name %s\n",root->childlist[0]->val.s,root->childlist[0]->name);
        printf("translate specifier error\n");
        return NULL;
    }
}
void translateProgram(Node *root)
{
    if (debug)
    {
        printf("translateprogram\n");
    }
    if (matchproduction(root, 1, "ExtDefList"))
    {
        translateExtDefList(root->childlist[0]);
    }
    else
    {
        printf("translate program error\n");
    }
}
void translateExtDefList(Node *root)
{
    if (debug)
    {
        printf("translateExtDefList\n");
    }
    if (root == NULL)
        return;
    if (matchproduction(root, 2, "ExtDef", "ExtDefList"))
    {
        translateExtDef(root->childlist[0]);
        translateExtDefList(root->childlist[1]);
    }
    else if (matchproduction(root, 2, "ExtDef", "NULL"))
    {
        translateExtDef(root->childlist[0]);
    }
    else
    {
        printf("translate ExtDefList error\n");
    }
}
void translateExtDef(Node *root)
{
    if (debug)
    {
        printf("translateExtDef \n");
        //printf("%s",root->childlist[0]->name);
        //printf("%s",root->childlist[1]->name);
    }
    if (matchproduction(root, 3, "Specifier", "ExtDecList", "SEMI"))
    {
        Type1 t = translateSpecifier(root->childlist[0]);
        if (t == NULL)
        {
            printf("error happend in extdef\n");
            return;
        }
        translateExtDecList(root->childlist[1], t);
    }
    else if (matchproduction(root, 2, "Specifier", "SEMI"))
    {
        translateSpecifier(root->childlist[0]);
    }
    else if (matchproduction(root, 3, "Specifier", "FunDec", "CompSt"))
    {
        Type1 t = translateSpecifier(root->childlist[0]);
        Type1 ret = (Type1)malloc(sizeof(struct Type_));
        if (t == NULL || ret == NULL)
        {
            printf("error happend in extdef\n");
            return;
        }
        ret->kind = t->kind;
        if (ret->kind == BASIC)
        {
            ret->u.basic = t->u.basic;
        }
        else if (ret->kind == ARRAY)
        {
            ret->u.array = t->u.array;
        }
        else
        {
            ret->u.structure = t->u.structure;
        }
        ret->rvalue = 1;
        Snode *func = translateFunDec(root->childlist[1], ret);
        if (func == NULL)
        {
            fprintf(stderr, "translate extdef err 1");
            return;
        }
        translateCompSt(root->childlist[2], func);
    }
    else
    {
        printf("translate ExtDef error\n");
    }
}
void translateCompSt(Node *root, Snode *func)
{ //CompSt表示一个由一对花括号括起来的语句块
    if (debug)
    {
        printf("translateCompSt\n");
    }
    if (matchproduction(root, 4, "LC", "DefList", "StmtList", "RC"))
    {
        translateDefList(root->childlist[1], varient);
        translateStmtList(root->childlist[2], func);
    }
    else if (matchproduction(root, 4, "LC", "NULL", "StmtList", "RC"))
    {
        translateStmtList(root->childlist[2], func);
    }
    else if (matchproduction(root, 4, "LC", "DefList", "NULL", "RC"))
    {
        translateDefList(root->childlist[1], varient);
    }
    else if (matchproduction(root, 4, "LC", "NULL", "NULL", "RC"))
    {
        //do nothing;
    }
    else
    {
        printf("translate CompSt error\n");
    }
}
void translateStmtList(Node *root, Snode *func)
{
    if (debug)
    {
        printf("translateStmtList\n");
    }
    if (root == NULL)
    {
        //printf("stmtlist is null\n");
        return;
    }
    if (matchproduction(root, 2, "Stmt", "StmtList"))
    {
        translateStmt(root->childlist[0], func);

        translateStmtList(root->childlist[1], func);
    }
    else if (matchproduction(root, 1, "Stmt"))
    {

        translateStmt(root->childlist[0], func);
    }
    else
    {
        printf("translate StmtList error\n");
    }
}
void translateStmt(Node *root, Snode *func)
{
    if (debug)
    {
        printf("translateStmt\n");
    }
    if (matchproduction(root, 2, "Exp", "SEMI"))
    {
        translateExp(root->childlist[0], NULL);
    }
    else if (matchproduction(root, 1, "CompSt"))
    {
        translateCompSt(root->childlist[0], func);
    }
    else if (matchproduction(root, 3, "RETURN", "Exp", "SEMI"))
    {
        Operand temp = newtemp();
        Type1 t1 = func->content.f->rettype;
        Type1 t2 = translateExp(root->childlist[1], temp);
        if (!equalType(t1, t2))
        {
            printsemanticerror(8, root->childlist[1]->lineno, "Type mismatched for return");
        }
        genintercode(IRETURN, temp);
    }
    else if (matchproduction(root, 5, "IF", "LP", "Exp", "RP", "Stmt"))
    {
        Operand label1 = newlabel();
        Operand label2 = newlabel();
        Type1 t = translatecond(root->childlist[2], label1, label2);
        genintercode(ILABEL, label1);
        if (t == NULL || t->kind != BASIC || t->u.basic != INT1)
        {
            printsemanticerror(7, root->childlist[2]->lineno, "Type mismatched in if");
        }
        translateStmt(root->childlist[4], func);
        genintercode(ILABEL, label2);
    }
    else if (matchproduction(root, 7, "IF", "LP", "Exp", "RP", "Stmt", "ELSE", "Stmt"))
    {
        Operand label1 = newlabel();
        Operand label2 = newlabel();
        Operand label3 = newlabel();
        Type1 t = translatecond(root->childlist[2], label1, label2);
        genintercode(ILABEL, label1);
        if (t == NULL || t->kind != BASIC || t->u.basic != INT1)
        {
            printsemanticerror(7, root->childlist[2]->lineno, "Type mismatched in if");
        }
        translateStmt(root->childlist[4], func);
        genintercode(IGOTO, label3);
        genintercode(ILABEL, label2);
        translateStmt(root->childlist[6], func);
        genintercode(ILABEL, label3);
    }
    else if (matchproduction(root, 5, "WHILE", "LP", "Exp", "RP", "Stmt"))
    {
        Operand label1 = newlabel();
        Operand label2 = newlabel();
        Operand label3 = newlabel();
        genintercode(ILABEL, label1);
        Type1 t = translatecond(root->childlist[2], label2, label3);
        if (t == NULL || t->kind != BASIC || t->u.basic != INT1)
        {
            printsemanticerror(7, root->childlist[2]->lineno, "Type mismatched in while");
        }
        genintercode(ILABEL, label2);
        translateStmt(root->childlist[4], func);
        genintercode(IGOTO, label1);
        genintercode(ILABEL, label3);
    }
    else
    {
        printf("translate StmtList error\n");
    }
}
//FunDec表示对一个函数头的定义。它包括一个表示函数名的标识符以及由一对圆括号括起来的一个形参列表
Snode *translateFunDec(Node *root, Type1 rettype)
{
    if (debug)
    {
        printf("translateFunDec\n");
    }

    if (matchproduction(root, 4, "ID", "LP", "VarList", "RP"))
    {
        //printf("bughere1\n");
        Node *id = root->childlist[0];
        Snode *sn = contain(id->val.s, function);
        if (sn != NULL)
        {
            printsemanticerror(4, id->lineno, "Redefined function");
            return sn;
        }
        else
        {

            sn = (Snode *)malloc(sizeof(Snode));
            sn->kind = function;
            sn->name = id->val.s;
            sn->next = NULL;
            int num = 0;
            int *p = &num;
            sn->content.f = (funcinfo *)malloc(sizeof(struct funcinfo_));
            sn->content.f->p = translateVarlist(root->childlist[2], p);
            sn->content.f->paramnum = num;
            sn->content.f->rettype = rettype;
            addtosymboltable(sn);
            return sn;
        }
    }
    else if (matchproduction(root, 3, "ID", "LP", "RP"))
    {
        Node *id = root->childlist[0];

        Snode *sn = contain(id->val.s, function);

        if (sn != NULL)
        {
            printsemanticerror(4, id->lineno, "Redefined function");
            return sn;
        }
        else
        {
            sn = (Snode *)malloc(sizeof(Snode));
            sn->kind = function;
            sn->name = id->val.s;

            sn->next = NULL;
            int num = 0;

            sn->content.f = (funcinfo *)malloc(sizeof(struct funcinfo_));
            sn->content.f->paramnum = num;

            sn->content.f->rettype = rettype;

            addtosymboltable(sn);
            return sn;
        }
    }
    else
    {
        printf("translate FunDec error\n");
    }
}
void translateExtDecList(Node *root, Type1 type)
{ //ExtDecList表示零个或多个对一个变量的定义VarDec
    if (debug)
    {
        printf("translateExtDecList\n");
    }
    if (matchproduction(root, 1, "VarDec"))
    {
        translateVarDec(root->childlist[0], varient, type, NULL);
    }
    else if (matchproduction(root, 3, "VarDec", "COMMA", "ExtDecList"))
    {
        translateVarDec(root->childlist[0], varient, type, NULL);
        translateExtDecList(root->childlist[2], type);
    }
    else
    {
        printf("translate ExtDecList error\n");
    }
}
Param *translateVarlist(Node *root, int *num)
{ //list of func
    if (debug)
    {
        printf("translateVarlist\n");
    }
    if (matchproduction(root, 3, "ParamDec", "COMMA", "VarList"))
    {
        Param *p = translateParamDec(root->childlist[0]);
        if (p == NULL)
        {
            printf("error happend in varlist\n");
            return NULL;
        }
        (*num)++;
        p->tail = translateVarlist(root->childlist[2], num);
        return p;
    }
    else if (matchproduction(root, 1, "ParamDec"))
    {
        *num = 1;
        return translateParamDec(root->childlist[0]);
    }
    else
    {
        printf("translate Varlist error\n");
    }
}
Param *translateParamDec(Node *root)
{ //每个ParamDec都是对一个形参的定义
    if (debug)
    {
        printf("translateParamDec\n");
    }
    if (matchproduction(root, 2, "Specifier", "VarDec"))
    {
        Type1 t = translateSpecifier(root->childlist[0]);
        if (t == NULL)
        {
            printf("error happend in paramdec\n");
            return NULL;
        }
        Snode *sn = translateVarDec(root->childlist[1], varient, t, NULL);
        if (sn == NULL)
        {
            printf("error happend in extdef\n");
            return NULL;
        }
        Param *p = (Param *)malloc(sizeof(Param));
        p->type = sn->content.type;
        p->tail = NULL;
        return p;
    }
    else
    {
        printf("translate ParamDec error\n");
    }
}
Type1 translatetype(Node *root)
{
    if (debug)
    {
        printf("translatetype\n");
    }
    if (equals(root->val.s, "int"))
    {
        Type1 res = (Type1)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->u.basic = INT1;
        res->rvalue = 0;
        return res;
    }
    else if (equals(root->val.s, "float"))
    {
        Type1 res = (Type1)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->u.basic = FLOAT1;
        res->rvalue = 0;
        return res;
    }
    else
    {
        printf("translate type error\n");
    }
}
Type1 translateStructspecifier(Node *root)
{ //比如struct Complex {…}，那么之后可以直接使用该结构体来定义变量，例如struct Complex a, b;
    if (debug)
    {
        printf("translateStructSpecifier,lineo:%d\n", root->lineno);
    }
    if (matchproduction(root, 2, "STRUCT", "Tag"))
    {
        Node *n = root->childlist[1]->childlist[0]; //the id node produced by Tag
        Snode *sn = contain(n->val.s, stru);
        if (sn == NULL)
        {
            printsemanticerror(17, n->lineno, "Undefined structure");
        }
        else
        {
            return sn->content.type;
        }
    }
    else if (matchproduction(root, 5, "STRUCT", "NULL", "LC", "DefList", "RC"))
    {
        anonymousnum++;
        char anoname[25];
        sprintf(anoname, "%d_anno", anonymousnum);
        Type1 st = (Type1)malloc(sizeof(struct Type_));
        st->kind = STRUCTURE;
        st->rvalue = 0;
        st->u.structure.strname = anoname;
        st->u.structure.fild = translateDefList(root->childlist[3], field);
        Snode *sn = createsnode(anoname, st, stru);
        addtosymboltable(sn);
        return st;
    }
    else if (matchproduction(root, 5, "STRUCT", "OptTag", "LC", "DefList", "RC"))
    { //structure definition
        Node *id = root->childlist[1]->childlist[0];
        Type1 st = (Type1)malloc(sizeof(struct Type_));
        st->kind = STRUCTURE;
        st->rvalue = 0;
        st->u.structure.strname = id->val.s;
        st->u.structure.fild = translateDefList(root->childlist[3], field);
        if (contain(id->val.s, stru))
        {
            printsemanticerror(16, id->lineno, "Duplicated name");
            return NULL;
        }
        else
        {
            Snode *sn = createsnode(id->val.s, st, stru);
            addtosymboltable(sn);
            return st;
        }

    } // should a struct without filed be allowed ? (when deflist-> empty) i dont consider it ,which can trigger error when program meets sturct a{};
    else
    {
        printf("translate Structspecifier error\n");
    }
}
FieldList translateDefList(Node *root, int skind)
{
    if (debug)
    {
        printf("translateDefList\n");
        //printf("%s----------\n",root->name);
    }
    if (matchproduction(root, 2, "Def", "DefList"))
    {
        FieldList f = translateDef(root->childlist[0], skind);
        FieldList fn = translateDefList(root->childlist[1], skind);
        f->tail = fn;
        return f;
    }
    else if (matchproduction(root, 1, "Def"))
    { // DefList -> Def, empty
        FieldList f = translateDef(root->childlist[0], skind);
        return f;
    }
    else
    {
        printf("translate DefList error\n");
        return NULL;
    }
}
FieldList translateDef(Node *root, int skind)
{ //int a, b, c; int a = 5;
    if (debug)
    {
        printf("translateDef\n");
    }
    if (matchproduction(root, 3, "Specifier", "DecList", "SEMI"))
    {
        Type1 t = translateSpecifier(root->childlist[0]);
        return translateDecList(root->childlist[1], skind, t);
    }
    else
    {

        printf("translate Def error\n");
        return NULL;
    }
}
FieldList translateDecList(Node *root, int skind, Type1 type)
{
    if (debug)
    {
        printf("translateDecList\n");
    }
    if (matchproduction(root, 1, "Dec"))
    {
        Snode *sn = translateDec(root->childlist[0], skind, type);
        if (sn == NULL)
            return NULL;
        FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
        f->tail = NULL;
        f->name = sn->name;
        f->type = sn->content.type;
        return f;
    }
    else if (matchproduction(root, 3, "Dec", "COMMA", "DecList"))
    {
        Snode *sn = translateDec(root->childlist[0], skind, type);
        if (sn == NULL)
            return NULL;
        FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
        f->tail = NULL;
        f->name = sn->name;
        f->type = sn->content.type;
        FieldList f1 = translateDecList(root->childlist[2], skind, type);
        if (f == NULL)
            return f1;
        f->tail = f1;
        return f;
    }
    else
    {
        printf("translate DecList error\n");
        return NULL;
    }
}
Snode *translateDec(Node *root, int skind, Type1 type)
{
    if (debug)
    {
        printf("translateDec\n");
    }
    if (matchproduction(root, 1, "VarDec"))
    {
        Operand v = newoperand(OVARIABLE, NULL);
        return translateVarDec(root->childlist[0], skind, type, v);
    }
    else if (matchproduction(root, 3, "VarDec", "ASSIGNOP", "Exp"))
    {
        Operand v = newoperand(OVARIABLE, NULL);
        Snode *sn = translateVarDec(root->childlist[0], skind, type, v);
        if (skind == varient)
        {
            Operand place = NULL;
            Type1 t = translateExp(root->childlist[2], place);
            if (!equalType(type, t))
            {
                printsemanticerror(5, root->childlist[0]->lineno, "Type mismatched for assignment");
            }
            genintercode(IASSIGN, v, place);
        }
        else if (skind == field)
        {
            printsemanticerror(15, root->childlist[0]->lineno, "initialize filed at struct definition");
        }
        else
        {
            printf("translate Dec error\n");
        }
        return sn;
    }
    else
    {
        printf("translate Dec error\n");
        return NULL;
    }
}
Snode *translateVarDec(Node *root, int skind, Type1 type, Operand v) //VarDec表示对一个变量的定义,该变量可以是一个标识符（例如int a中的a），也可以是一个标识符后面跟着若干对方括号括起来的数字
{
    if (debug)
    {
        printf("translateVarDec\n");
    }
    if (matchproduction(root, 1, "ID")) //int a;
    {
        Node *id = root->childlist[0];
        Snode *sn = createsnode(id->val.s, type, skind);
        if (contain(id->val.s, skind) != NULL)
        { //结构体中的域不与变量重名，并且不同结构体中的域互不重名。
            switch (skind)
            {
            case varient:
                printsemanticerror(3, id->lineno, "Redefined variable");
                break;
            case field:
                printsemanticerror(15, id->lineno, "Redefined field");
                return NULL;
                break;
            default:
                printf("unknown problem in vardec translate\n");
                break;
            }
        }
        else
        { // insert to symboltable
            if (v == NULL)
            {
                //nothing
            }
            else
            {
                v->u.name = id->val.s;
                if (type->kind == ARRAY || type->kind == STRUCTURE)
                {
                    int size = getsize(type);
                    genintercode(IDEC,v,size);
                }
            }

            addtosymboltable(sn);
        }
        return sn;
    }
    else if (matchproduction(root, 4, "VarDec", "LB", "INT", "RB")) //int a [x][y]
    {
        Snode *sn = translateVarDec(root->childlist[0], skind, type, v);
        if (sn != NULL)
        {
            Type1 arrayt = (Type1)(malloc(sizeof(struct Type_)));
            arrayt->kind = ARRAY;
            arrayt->u.array.elem = sn->content.type;
            arrayt->u.array.size = root->childlist[2]->val.i;
            arrayt->rvalue = 0;
            sn->content.type = arrayt;
        }
        return sn;
    }
    else
    {
        printf("translate VarDec error\n");
        return NULL;
    }
}
Param *translateArgs(Node *root)
{
    if (debug)
    {
        printf("translateArgs\n");
    }
    if (matchproduction(root, 1, "Exp"))
    {
        Operand temp = newtemp();
        Param *arg = (Param *)malloc(sizeof(struct Param_));
        arg->type = translateExp(root->childlist[0], temp);
        arg->op = temp;
        arg->tail = NULL;
        return arg;
    }
    else if (matchproduction(root, 3, "Exp", "COMMA", "Args"))
    {
        Operand temp = newtemp();
        Param *arg = (Param *)malloc(sizeof(struct Param_));
        arg->type = translateExp(root->childlist[0], temp);
        arg->op = temp;
        Param *argtail = translateArgs(root->childlist[2]);
        arg->tail = argtail;
        return arg;
    }
}
Type1 translatecond(Node *root, Operand labeltrue, Operand labelfalse)
{
    if (matchproduction(root, 3, "Exp", "RELOP", "Exp"))
    {
        Operand t1 = newtemp();
        Operand t2 = newtemp();
        Type1 le = translateExp(root->childlist[0], t1);
        Type1 ri = translateExp(root->childlist[2], t2);
        if (!(le != NULL && le->kind == BASIC && le->u.basic == INT1 && ri != NULL && ri->kind == BASIC && ri->u.basic == INT1))
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
        }
        Type1 res = (Type1)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->rvalue = 1;
        res->u.basic = INT1;
        Operand op = newoperand(ORELOP, root->childlist[1]->val.s);
        genintercode(IIFGOTO, t1, op, t2, labeltrue);
        genintercode(IGOTO, labelfalse);
        return res;
    }
    else if (matchproduction(root, 2, "NOT", "Exp"))
    {
        return translatecond(root->childlist[1], labelfalse, labeltrue);
    }
    else if (matchproduction(root, 3, "Exp", "AND", "Exp"))
    {
        Operand label1 = newlabel();
        translatecond(root->childlist[0], label1, labelfalse);
        genintercode(ILABEL, label1);
        translatecond(root->childlist[2], labeltrue, labelfalse);
        Type1 res = (Type1)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->rvalue = 1;
        return res;
    }
    else if (matchproduction(root, 3, "Exp", "AND", "Exp"))
    {
        Operand label1 = newlabel();
        translatecond(root->childlist[0], labeltrue, label1);
        genintercode(ILABEL, label1);
        translatecond(root->childlist[2], labeltrue, labelfalse);
        Type1 res = (Type1)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->rvalue = 1;
        return res;
    }
    else
    {
        Operand t1 = newtemp();
        Type1 res = translateExp(root, t1);
        Operand notsameop = newoperand(ORELOP, "!=");
        Operand const0 = newoperand(OCONSTANT, 0);
        genintercode(IIFGOTO, t1, notsameop, const0, labeltrue);
        genintercode(IGOTO, labelfalse);
        return res;
    }
}
Type1 translateExp(Node *root, Operand place)
{
    if (root == NULL)
    {
        printf("exp root is null?\n");
        return NULL;
    }
    if (debug)
    {
        printf("translateExp  lino :%d\n", root->lineno);
    }

    if (matchproduction(root, 3, "Exp", "ASSIGNOP", "Exp"))
    {
        //LAB3
        
        Operand t1 = newtemp();
        Type1 ri = translateExp(root->childlist[2], t1); //code1 inside
        Operand vari = newoperand(OVARIABLE, root->childlist[0]->childlist[0]->val.s);
        Type1 le = translateExp(root->childlist[0], vari);
        if (le != NULL && le->rvalue)
        {
            printsemanticerror(6, root->childlist[0]->lineno, "The left-hand side of an assignment must be a variable");
        }
        if (!equalType(le, ri))
        {
            printsemanticerror(5, root->childlist[0]->lineno, "Type mismatched for assignment");
        }

        //lab3
        
        genintercode(IASSIGN, vari, t1);
        //genintercode(IASSIGN, place, vari);       place is null heres
        //

        if (le != NULL)
        {
            return le;
        }
        else
        {

            return ri;
        }
    }
    else if (matchproduction(root, 3, "Exp", "AND", "Exp") || matchproduction(root, 3, "Exp", "OR", "Exp") || matchproduction(root, 3, "Exp", "RELOP", "Exp") || matchproduction(root, 2, "NOT", "Exp"))
    {
        //仅有int型变量才能进行逻辑运算或者作为if和while语句的条件
        Operand label1 = newlabel();
        Operand label2 = newlabel();
        Operand const0 = newoperand(OCONSTANT, 0);
        genintercode(IASSIGN, place, const0);

        Type1 res = translatecond(root, label1, label2);
        genintercode(ILABEL, label1);
        Operand const1 = newoperand(OCONSTANT, 1);
        genintercode(IASSIGN, place, const1);
        genintercode(ILABEL, label2);
        return res;
    }
    else if (matchproduction(root, 3, "Exp", "PLUS", "Exp") || matchproduction(root, 3, "Exp", "MINUS", "Exp") || matchproduction(root, 3, "Exp", "STAR", "Exp") || matchproduction(root, 3, "Exp", "DIV", "Exp"))
    {
        //lab3
        Operand t1 = newtemp();
        Operand t2 = newtemp();

        Type1 le = translateExp(root->childlist[0], t1);
        Type1 ri = translateExp(root->childlist[2], t2);

        if ((!(le != NULL && le->kind == BASIC && le->u.basic == INT1 && ri != NULL && ri->kind == BASIC && ri->u.basic == INT1)) && (!(le != NULL && le->kind == BASIC && le->u.basic == FLOAT1 && ri != NULL && ri->kind == BASIC && ri->u.basic == FLOAT1)))
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
        }
        //LAB3
        switch (root->childlist[1]->type)
        {
        case PLUS:
            genintercode(IADD, place, t1, t2);
            break;
        case MINUS:
            genintercode(ISUB, place, t1, t2);
            break;
        case STAR:
            genintercode(IMUL, place, t1, t2);
            break;
        case DIV:
            genintercode(IDIV, place, t1, t2);
            break;
        default:
            printf("translate exp error!\n");
            break;
        }

        if (le != NULL)
        {
            return le;
        }
        else
            return ri;
    }
    else if (matchproduction(root, 3, "LP", "Exp", "RP"))
    {
        return translateExp(root->childlist[1], place);
    }
    else if (matchproduction(root, 2, "MINUS", "Exp"))
    {
        Operand t1 = newtemp();
        Type1 t = translateExp(root->childlist[1], t1);
        if (t != NULL && t->kind != BASIC)
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
        }
        Operand CONS0 = newoperand(OCONSTANT, 0);
        genintercode(ISUB, place, CONS0, t1);
        return t;
    }

    else if (matchproduction(root, 4, "ID", "LP", "Args", "RP"))
    {
        Node *id = root->childlist[0];
        Snode *sn = contain(id->val.s, function);
        if (contain(id->val.s, varient) != NULL) //not a function ,but a varient
        {
            printsemanticerror(11, id->lineno, "not a function");
            return NULL;
        }
        else if (sn != NULL)
        {
            Param *p = translateArgs(root->childlist[2]);
            if (!checkargs(p, sn->content.f->p, sn->content.f->paramnum))
            {
                printsemanticerror(9, id->lineno, "func param type or num not matched");
            }

            //lab3
            if (strcmp(id->val.s, "write") == 0)
            {
                genintercode(IWRITE, p->op);
                Operand CONST0 = newoperand(OCONSTANT, 0);
                genintercode(IASSIGN, place, CONST0);
            }
            else
            {
                while (p != NULL)
                {
                    genintercode(IARG, p->op);
                    p = p->tail;
                }
                Operand f = newoperand(OFUNCTION, id->val.s);
                genintercode(ICALL, place, f);
            }

            return sn->content.f->rettype;
        }
        else
        {
            printsemanticerror(2, id->lineno, "Undefined function");
            return NULL;
        }
    }
    else if (matchproduction(root, 3, "ID", "LP", "RP"))
    { //func()
        Node *id = root->childlist[0];
        Snode *sn = contain(id->val.s, function);
        if (contain(id->val.s, varient) != NULL)
        {
            printsemanticerror(11, id->lineno, "not a function");
            return NULL;
        }
        else if (sn != NULL)
        {
            if (strcmp(id->val.s, "read") == 0)
            {
                genintercode(IREAD, place);
            }
            else
            {
                Operand f = newoperand(OFUNCTION, id->val.s);
                genintercode(ICALL, place, f);
            }
            return sn->content.f->rettype;
        }
        else
        {
            printsemanticerror(2, id->lineno, "Undefined function");
            return NULL;
        }
    }
    else if (matchproduction(root, 4, "Exp", "LB", "Exp", "RB"))
    { //数组访问表达式
        Operand base = newtemp();
        Type1 t = translateExp(root->childlist[0], base);
        if (t == NULL)
            return NULL;
        if (t->kind != ARRAY)
        {
            printsemanticerror(10, root->childlist[0]->lineno, "not an array");
        }
        Operand index = newtemp();
        Type1 t1 = translateExp(root->childlist[2], index);
        if (t1 == NULL)
            return NULL;
        if (t1->kind != BASIC || t1->u.basic != INT1)
        {
            printsemanticerror(12, root->childlist[2]->lineno, "num in [] not an integer");
        }
        int size = getsize(t->u.array.elem);
        Operand CONSTSIZE = newoperand(OCONSTANT, size);
        Operand offset = newtemp();
        if (index->u.value != 0)
            genintercode(IMUL, offset, index, CONSTSIZE);
        Operand baseaddr = newtemp();
        genintercode(IGETADDR, baseaddr, base);
        place->kind = OADDRESS;
        genintercode(IADD, place, baseaddr, offset);

        return NULL;
    }
    else if (matchproduction(root, 3, "Exp", "DOT", "ID"))
    {
        Operand temp = newtemp();
        Type1 t = translateExp(root->childlist[0], temp);
        Operand tempaddr = newtemp();
        genintercode(IGETADDR, tempaddr, temp);

        if (t == NULL)
            return NULL;
        if (t->kind != STRUCTURE)
        {
            printsemanticerror(13, root->childlist[0]->lineno, "Illegal use of \".\"");
            return NULL;
        }
        Node *id = root->childlist[2];
        Snode *sn = contain(t->u.structure.strname, stru); //problem lies here, t ,instead of id should be used as parm
        if (sn == NULL)
        {
            printf("translate Exp error\n");
            return NULL;
        }
        FieldList f = t->u.structure.fild;
        int offset = 0;
        while (f != NULL)
        {

            if (equals(id->val.s, f->name))
            {
                Operand constoffset = newoperand(OCONSTANT, offset);
                place->kind = OADDRESS;
                genintercode(IADD, place, tempaddr, constoffset);
                return f->type;
            }
            offset += getsize(f->type);
            f = f->tail;
        }
        printsemanticerror(14, id->lineno, "Non-existent field");
        return NULL;
    }
    else if (matchproduction(root, 1, "ID"))
    {
        Node *id = root->childlist[0];
        Snode *sn = contain(id->val.s, varient);
        if (sn != NULL)
        {
            //lab3:
            Operand vari = newoperand(OVARIABLE, id->val.s);
            genintercode(IASSIGN, place, vari);
            //
            return sn->content.type;
        }
        else
        {
            printsemanticerror(1, id->lineno, "Undefined variable");
            return NULL;
        }
    }
    else if (matchproduction(root, 1, "INT"))
    {
        Type1 t = (Type1)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->rvalue = 1;
        t->u.basic = INT1;

        //lab3:
        int value = root->childlist[0]->val.i;
        Operand v = newoperand(OCONSTANT, value);

        if (place == NULL)
        {
            place = v;
        }
        else
        {
            genintercode(IASSIGN, place, v);
        }
        return t;
    }
    else if (matchproduction(root, 1, "FLOAT")) //lab3 not consider this
    {
        Type1 t = (Type1)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->rvalue = 1;
        t->u.basic = FLOAT1;
        return t;
    }
    else
    {
        printf("translate Exp error\n");
    }
}
int checkargs(Param *p1, Param *p2, int num)
{ // p1 to be matched against existed p2
    for (int i = 0; i < num; i++)
    {
        if (p1 == NULL || p2 == NULL)
        {
            return 0;
        }
        if (!equalType(p1->type, p2->type))
        {
            return 0;
        }
        p1 = p1->tail;
        p2 = p2->tail;
    }
    if (p1 != NULL || p2 != NULL)
    {
        return 0;
    }
    return 1;
}
void printsemanticerror(int errornumber, int line, char *msg)
{
    printf("Error type %d at Line %d: %s.\n", errornumber, line, msg);
}