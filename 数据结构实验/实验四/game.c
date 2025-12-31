#include "game.h"
#include <conio.h>

// 实时读取按键
static char getMove()
{
    int ch = _getch();

    if (ch == 0 || ch == 0xE0)
    {
        int arrow = _getch();
        switch (arrow)
        {
        case 72:
            return 'w'; // ↑
        case 80:
            return 's'; // ↓
        case 75:
            return 'a'; // ←
        case 77:
            return 'd'; // →
        default:
            return 0;
        }
    }

    return (char)ch;
}

void GameLoop(MazeType *maze, PosType start, PosType end)
{
    PosType playerPos = start;
    Stack solutionPath;
    InitStack(&solutionPath);

    bool showSolution = false;
    bool running = true;
    bool win = false;
    int steps = 0;

    while (running)
    {
        clearScreen();

        PrintMaze(maze, playerPos, start, end, showSolution ? &solutionPath : NULL);

        if (Same(playerPos, end))
        {
            win = true;
            break;
        }

        printf("\n控制: W/A/S/D 或方向键移动\n");
        printf("选项: H 显示/隐藏提示, Q 返回主菜单\n");
        printf("当前步数: %d\n", steps);
        printf("按键继续...\n");

        char move = getMove();
        if (move == 0)
        {
            continue;
        }

        PosType nextPos = playerPos;

        switch (move)
        {
        case 'w':
        case 'W':
            nextPos.r--;
            break;
        case 'a':
        case 'A':
            nextPos.c--;
            break;
        case 's':
        case 'S':
            nextPos.r++;
            break;
        case 'd':
        case 'D':
            nextPos.c++;
            break;

        case 'h':
        case 'H':
            showSolution = !showSolution;
            if (showSolution)
            {
                MazeType *mazeCopy = (MazeType *)malloc(sizeof(MazeType));
                InitMaze(mazeCopy, maze->m, maze->n);
                for (int i = 0; i < maze->m; i++)
                    memcpy(mazeCopy->arr[i], maze->arr[i], maze->n * sizeof(char));

                DestroyStack(&solutionPath);

                if (!MazePath(mazeCopy, playerPos, end, &solutionPath))
                {
                    printf("\n(从当前位置没有找到路径)\n");
                    showSolution = false;
                }

                DestroyMaze(mazeCopy);
                free(mazeCopy);
            }
            else
            {
                DestroyStack(&solutionPath);
            }
            break;

        case 'q':
        case 'Q':
            running = false;
            break;

        default:
            break;
        }

        // 碰撞检测
        if (nextPos.r >= 0 && nextPos.r < maze->m &&
            nextPos.c >= 0 && nextPos.c < maze->n &&
            maze->arr[nextPos.r][nextPos.c] != WALL)
        {
            if (!Same(playerPos, nextPos))
            {
                playerPos = nextPos;
                steps++;
            }
        }
    }

    DestroyStack(&solutionPath);

    // 通关
    if (win)
    {
        clearScreen();
        printf("========================================\n");
        printf("           恭喜通关迷宫!               \n");
        printf("========================================\n\n");
        PrintMaze(maze, playerPos, start, end, NULL);
        printf("\n你总共移动了 %d 步。\n", steps);
        printf("按任意键返回主菜单...\n");
        getMove();
    }
}
