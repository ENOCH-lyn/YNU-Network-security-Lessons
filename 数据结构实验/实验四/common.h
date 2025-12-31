#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef __cplusplus
typedef enum
{
    false,
    true
} bool;
#endif

#define WALL '#'
#define PATH ' '
#define PLAYER '@'
#define START 'S'
#define END 'E'
#define VISITED '+'
#define DEAD_END 'x'

typedef struct
{
    int r;
    int c;
} PosType;

typedef struct
{
    int m;
    int n;
    char **arr;
} MazeType;

void clearScreen();
bool Same(PosType a, PosType b);

#endif
