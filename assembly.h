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
void pusha(FILE *f);
void popa(FILE *f);
void printobjectcode(FILE *f, inode *p);
#endif
