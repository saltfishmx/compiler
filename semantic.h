#ifndef semantic
#define semantic
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "symbol.h"
#include "tree.h"
int equals(char *s1, char *s2);
int matchproduction(Node *root, int num, ...);

#endif