#pragma once
#include <assert.h>
#include <stddef.h>

typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

void *checked_malloc(int);
void *checked_realloc(void *, size_t);
string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

string String_format(const char *s, ...);
string String_from_int(int n);