#include "common.h"
#include "stack.h"
#include "maze.h"
#include "game.h"

#define MAZE_WIDTH 21
#define MAZE_HEIGHT 15

#include <windows.h>

void RunAutoSolver(MazeType *maze, PosType start, PosType end)
{
    clearScreen();

    MazeType *mazeCopy = (MazeType *)malloc(sizeof(MazeType));
    InitMaze(mazeCopy, maze->m, maze->n);
    for (int i = 0; i < maze->m; i++)
        memcpy(mazeCopy->arr[i], maze->arr[i], maze->n * sizeof(char));

    Stack solutionPath;
    bool found = MazePath(mazeCopy, start, end, &solutionPath);

    if (found)
    {
        PrintMaze(mazeCopy, (PosType){-1, -1}, start, end, &solutionPath);
        printf("\n");
    }
    else
    {
        printf("没有找到路径\n");
        PrintMaze(mazeCopy, (PosType){-1, -1}, start, end, &solutionPath);
    }

    DestroyStack(&solutionPath);
    DestroyMaze(mazeCopy);
    free(mazeCopy);

    printf("\n按 Enter 键返回主菜单...");
    while (getchar() != '\n')
        ;
}

int main()
{
    SetConsoleOutputCP(65001);

    MazeType maze;
    PosType start, end;

    InitMaze(&maze, MAZE_HEIGHT, MAZE_WIDTH);
    GenerateMaze(&maze, &start, &end);

    char choice;
    do
    {
        clearScreen();

        PrintMaze(&maze, (PosType){-1, -1}, start, end, NULL);

        printf("\n--- 主菜单 ---\n");
        printf("1. 开始\n");
        printf("2. 求解迷宫\n");
        printf("3. 新迷宫\n");
        printf("Q. 退出\n");
        printf("请输入选项: ");

        scanf(" %c", &choice);
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case '1':
            GameLoop(&maze, start, end);
            break;
        case '2':
            RunAutoSolver(&maze, start, end);
            break;
        case '3':
            GenerateMaze(&maze, &start, &end);
            break;
        case 'q':
        case 'Q':
            break;
        default:
            break;
        }

    } while (choice != 'q' && choice != 'Q');

    DestroyMaze(&maze);
    return 0;
}
