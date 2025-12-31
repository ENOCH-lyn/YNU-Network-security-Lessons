#ifndef MAZE_H
#define MAZE_H

#include "common.h"
#include "stack.h"

// 初始化迷宫
void InitMaze(MazeType *maze, int m, int n);

// 销毁迷宫
void DestroyMaze(MazeType *maze);

// 自动生成迷宫
void GenerateMaze(MazeType *maze, PosType *start, PosType *end);

// 打印迷宫
void PrintMaze(const MazeType *maze, PosType player, PosType start, PosType end, const Stack *solutionPath);

// 求解迷宫路径
bool MazePath(MazeType *maze, PosType start, PosType end, Stack *S);

// 获取下一个位置
PosType NextPos(PosType pos, int di);

#endif