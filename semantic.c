
#include "semantic.h"
int anonymousnum = 0;
extern Snode symboltable[];
int equals(char *s1, char *s2)
{
    if (strcmp(s1, s2) == 0)
    {
        return 1;
    }
    else
        return 0;
}
int equalType(Type t1, Type t2)
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
        if (root->childlist[i] == NULL)
        {
            res = 0;
            break;
        }
        char *name = va_arg(valist, char *);
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
Type translateSpecifier(Node *root)
{
    if (matchproduction(root, 1, "Type"))
    {
        return translatetype(root->childlist[0]);
    }
    else if (matchproduction(root, 1, "StructSpecifier"))
    {
        return translateStructspecifier(root->childlist[0]);
    }
    else
    {
        printf("translate specifier error\n");
        return NULL;
    }
}
Type translatetype(Node *root)
{
    if (equals(root->val.s, "int"))
    {
        Type res = (Type)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->u.basic = INT;
        res->rvalue = 0;
        return res;
    }
    else if (equals(root->val.s, "float"))
    {
        Type res = (Type)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->u.basic = FLOAT;
        res->rvalue = 0;
        return res;
    }
    else
    {
        printf("translate type error\n");
    }
}
Type translateStructspecifier(Node *root)
{ //比如struct Complex {…}，那么之后可以直接使用该结构体来定义变量，例如struct Complex a, b;
    if (matchproduction(root, 2, "STRUCT", "Tag"))
    {
        Node *n = root->childlist[1]->childlist[0]; //the id node produced by Tag
        Snode *sn = contain(n->name, stru);
        if (sn == NULL)
        {
            printsemanticerror(17, n->lineno, "Undefined structure");
        }
        else
        {
            return sn->content.type;
        }
    }
    else if (matchproduction(root, 4, "STRUCT", "LC", "DefList", "RC"))
    {
        anonymousnum++;
        char anoname[25];
        sprintf(anoname, "%d_anno", anonymousnum);
        Type st = (Type)malloc(sizeof(struct Type_));
        st->kind = STRUCTURE;
        st->rvalue = 0;
        st->u.structure.strname = anoname;
        st->u.structure.fild = translateDefList(root->childlist[2],field);
    }
    else if (matchproduction(root, 5, "STRUCT", "OptTag", "LC", "DefList", "RC"))
    { //structure definition
        Node *id = root->childlist[1]->childlist[0];
        Type st = (Type)malloc(sizeof(struct Type_));
        st->kind = STRUCTURE;
        st->rvalue = 0;
        st->u.structure.strname = id->name;
        st->u.structure.fild = translateDefList(root->childlist[3],field);
        if (contain(id->name, stru))
        {
            printsemanticerror(16, id->lineno, "Duplicated name");
        }
        else
        {
            Snode *sn = createsnode(id->name, st, stru);
            addtosymboltable(sn);
        }
    } // should a struct without filed be allowed ? (when deflist-> empty) i dont consider it ,which can trigger error when program meets sturct a{};
    else
    {
        printf("translate Structspecifier error\n");
    }
}
FieldList translateDefList(Node *root,int skind)
{
    if (matchproduction(root, 2, "Def", "DefList"))
    {
        FieldList f = translateDef(root->childlist[0],skind);
        FieldList fn = translateDefList(root->childlist[1],skind);
        f->tail = fn;
        return f;
    }
    else if (matchproduction(root, 1, "Def"))
    { // DefList -> Def, empty
        FieldList f = translateDef(root->childlist[0],skind);
        return f;
    }
    else
    {
        printf("translate DefList error\n");
    }
}
FieldList translateDef(Node *root,int skind)
{ //int a, b, c; int a = 5;
    if (matchproduction(root, 3, "Specifier", "DecList", "SEMI"))
    {
        Type t = translateSpecifier(root->childlist[0]);
        return translateDecList(root->childlist[1],skind,t);
    }
    else
    {
        printf("translate Def error\n");
    }
}
FieldList translateDecList(Node *root,int skind ,Type type){
    if(matchproduction(root,1,"Dec")){
        Snode *sn = translateDec(root->childlist[0],skind,type);
        FieldList f  = (FieldList)malloc(sizeof(struct FieldList_));
        f->tail =NULL;
        f->name = sn->name;
        f->type = sn->content.type;
        return f;
    }
    else if(matchproduction(root,3,"Dec","COMMA","DecList")){
        Snode *sn = translateDec(root->childlist[0],skind,type);
        FieldList f  = (FieldList)malloc(sizeof(struct FieldList_));
        f->tail =NULL;
        f->name = sn->name;
        f->type = sn->content.type;
        FieldList f1 = translateDecList(root->childlist[2],skind,type);
        if(f==NULL) return f1;
        f->tail = f1;
        return f;
    }
    else{
        printf("translate DecList error\n");
    }
}
Snode *translateDec(Node *root, int skind, Type type)
{
    if (matchproduction(root, 1, "VarDec"))
    {
        return translateVarDec(root->childlist[0], skind, type);
    }
    else if (matchproduction(root, 3, "VarDec", "ASSIGNOP", "Exp"))
    {
        Snode *sn = translateVarDec(root->childlist[0], skind, type);
        if (skind == varient)
        {
            Type t = translateExp(root->childlist[2]);
            if(!equalType(type,t)){
                printsemanticerror(5,root->childlist[0]->lineno,"Type mismatched for assignment");
            }
        }
        else if(skind == field){
            printsemanticerror(15,root->childlist[0]->lineno,"initialize filed at struct definition");
        }
        else
        {
            printf("translate Dec error\n");
        }
    }
    else
    {
        printf("translate Dec error\n");
    }
}
Snode *translateVarDec(Node *root, int skind, Type type)
{
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
            addtosymboltable(sn);
        }
        return sn;
    }
    else if (matchproduction(root, 4, "VarDec", "LB", "INT", "RB")) //int a [x][y]
    {
        Snode *sn = translateVarDec(root->childlist[0], skind, type);
        if (sn != NULL)
        {
            Type arrayt = (Type)(malloc(sizeof(struct Type_)));
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
    }
}
Param *translateArgs(Node *root)
{
    if (matchproduction(root, 1, "Exp"))
    {
        Param *arg = (Param *)malloc(sizeof(struct Param_));
        arg->type = translateExp(root->childlist[0]);
        arg->tail = NULL;
        return arg;
    }
    else if (matchproduction(root, 3, "Exp", "COMMA", "Args"))
    {
        Param *arg = (Param *)malloc(sizeof(struct Param_));
        arg->type = translateExp(root->childlist[0]);
        Param *argtail = translateArgs(root->childlist[2]);
        arg->tail = argtail;
        return arg;
    }
}
Type translateExp(Node *root)
{
    if (matchproduction(root, 3, "Exp", "ASSIGNOP", "Exp"))
    {
        Type le = translateExp(root->childlist[0]);
        Type ri = translateExp(root->childlist[2]);
        if (le != NULL && le->rvalue)
        {
            printsemanticerror(6, root->childlist[0]->lineno, "The left-hand side of an assignment must be a variable");
        }
        if (!equalType(le, ri))
        {
            printsemanticerror(5, root->childlist[0]->lineno, "Type mismatched for assignment");
        }
        if (le != NULL)
        {
            return le;
        }
        else
            return ri;
    }
    else if (matchproduction(root, 3, "Exp", "AND", "Exp") || matchproduction(root, 3, "Exp", "OR", "Exp") || matchproduction(root, 3, "Exp", "RELOP", "Exp"))
    {
        //仅有int型变量才能进行逻辑运算或者作为if和while语句的条件
        Type le = translateExp(root->childlist[0]);
        Type ri = translateExp(root->childlist[2]);
        if (!(le != NULL && le->kind == BASIC && le->u.basic == INT && ri != NULL && ri->kind == BASIC && ri->u.basic == INT))
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
        }
        Type res = (Type)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->rvalue = 1;
        res->u.basic = INT;
        return res;
    }
    else if (matchproduction(root, 3, "Exp", "PLUS", "Exp") || matchproduction(root, 3, "Exp", "MINUS", "Exp") || matchproduction(root, 3, "Exp", "STAR", "Exp") || matchproduction(root, 3, "Exp", "DIV", "Exp"))
    {
        Type le = translateExp(root->childlist[0]);
        Type ri = translateExp(root->childlist[2]);
        if ((!(le != NULL && le->kind == BASIC && le->u.basic == INT && ri != NULL && ri->kind == BASIC && ri->u.basic == INT)) && (!(le != NULL && le->kind == BASIC && le->u.basic == FLOAT && ri != NULL && ri->kind == BASIC && ri->u.basic == FLOAT)))
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
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
        return translateExp(root->childlist[1]);
    }
    else if (matchproduction(root, 2, "MINUS", "Exp"))
    {
        Type t = translateExp(root->childlist[1]);
        if (t != NULL && t->kind != BASIC)
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
        }
        return t;
    }
    else if (matchproduction(root, 2, "NOT", "Exp"))
    {
        Type t = translateExp(root->childlist[1]);
        if (t != NULL && t->kind != BASIC)
        {
            printsemanticerror(7, root->childlist[0]->lineno, "Type mismatched for operands.");
        }
        Type res = (Type)malloc(sizeof(struct Type_));
        res->kind = BASIC;
        res->rvalue = 1;
        res->u.basic = INT;
        return res;
    }
    else if (matchproduction(root, 4, "ID", "LP", "Args", "RP"))
    {
        Node *id = root->childlist[0];
        Snode *sn = contain(id->name, stru);
        if (contain(id->name, varient) != NULL)
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
        Snode *sn = contain(id->name, stru);
        if (contain(id->name, varient) != NULL)
        {
            printsemanticerror(11, id->lineno, "not a function");
            return NULL;
        }
        else if (sn != NULL)
        {

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
        Type t = translateExp(root->childlist[0]);
        if (t == NULL)
            return NULL;
        if (t->kind != ARRAY)
        {
            printsemanticerror(10, root->childlist[0]->lineno, "not an array");
        }
        Type t1 = translateExp(root->childlist[2]);
        if (t1 == NULL)
            return NULL;
        if (t1->kind != BASIC || t1->u.basic != INT)
        {
            printsemanticerror(12, root->childlist[2]->lineno, "num in [] not an integer");
        }
    }
    else if (matchproduction(root, 3, "Exp", "DOT", "ID"))
    {
        Type t = translateExp(root->childlist[0]);
        if (t == NULL)
            return NULL;
        if (t->kind != STRUCTURE)
        {
            printsemanticerror(13, root->childlist[0]->lineno, "Illegal use of \".\"");
            return NULL;
        }
        Node *id = root->childlist[2];
        Snode *sn = contain(id->val.s, stru);
        if (sn == NULL)
        {
            printf("translate Exp error\n");
            return NULL;
        }
        FieldList f = t->u.structure.fild;
        while (f != NULL)
        {
            if (equals(id->val.s, f->name))
            {

                return f->type;
            }
            f = f->tail;
        }
        printsemanticerror(14, id->lineno, "Non-existent field");
        return NULL;
    }
    else if (matchproduction(root, 1, "ID"))
    {
        Node *id = root->childlist[0];
        Snode *sn = contain(id->name, varient);
        if (sn != NULL)
        {
            return sn->content.type;
        }
        else
        {
            printsemanticerror(1, id->lineno, "Undefined variable");
        }
    }
    else if (matchproduction(root, 1, "INT"))
    {
        Type t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->rvalue = 1;
        t->u.basic = INT;
        return t;
    }
    else if (matchproduction(root, 1, "FLOAT"))
    {
        Type t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->rvalue = 1;
        t->u.basic = FLOAT;
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