#ifndef assembly
#define assembly
#include "inter.h"
typedef struct varipair_ * varipair;
struct varipair_
{
    int offset;
    char *s;
};
void initregi();
void clearreg();
int reg(char *variname, FILE *f);
void printassembly(FILE *f, inode *p);
int pusha(FILE *f);
void popa(FILE *f,int count);
void printobjectcode(FILE *f, inode *p);
#endif
