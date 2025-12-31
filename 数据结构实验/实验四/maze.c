#include "maze.h"

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#endif
}

bool Same(PosType a, PosType b)
{
    return a.r == b.r && a.c == b.c;
}

void InitMaze(MazeType *maze, int m, int n)
{
    maze->m = m;
    maze->n = n;

    maze->arr = (char **)malloc(m * sizeof(char *));
    for (int i = 0; i < m; i++)
    {
        maze->arr[i] = (char *)malloc(n * sizeof(char));
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            maze->arr[i][j] = WALL;
}

void DestroyMaze(MazeType *maze)
{
    for (int i = 0; i < maze->m; i++)
    {
        free(maze->arr[i]);
    }
    free(maze->arr);
    maze->m = 0;
    maze->n = 0;
}

static void carve(MazeType *maze, int r, int c)
{
    maze->arr[r][c] = PATH;
    int dirs[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++)
    {
        int j = rand() % 4;
        int tmp = dirs[i];
        dirs[i] = dirs[j];
        dirs[j] = tmp;
    }

    for (int i = 0; i < 4; i++)
    {
        int dr = 0, dc = 0;
        int nr, nc, nnr, nnc;

        switch (dirs[i])
        {
        case 1:
            dc = 1;
            break;
        case 2:
            dr = 1;
            break;
        case 3:
            dc = -1;
            break;
        case 4:
            dr = -1;
            break;
        }

        nr = r + dr;
        nc = c + dc;
        nnr = r + dr * 2;
        nnc = c + dc * 2;

        if (nnr > 0 && nnr < maze->m - 1 &&
            nnc > 0 && nnc < maze->n - 1 &&
            maze->arr[nnr][nnc] == WALL)
        {
            maze->arr[nr][nc] = PATH;
            carve(maze, nnr, nnc);
        }
    }
}

void GenerateMaze(MazeType *maze, PosType *start, PosType *end)
{
    srand((unsigned int)time(NULL));

    if (maze->m % 2 == 0)
        maze->m--;
    if (maze->n % 2 == 0)
        maze->n--;
    if (maze->m < 5)
        maze->m = 5;
    if (maze->n < 5)
        maze->n = 5;

    for (int i = 0; i < maze->m; i++)
        for (int j = 0; j < maze->n; j++)
            maze->arr[i][j] = WALL;

    carve(maze, 1, 1);

    *start = (PosType){1, 1};
    *end = (PosType){maze->m - 2, maze->n - 2};
    maze->arr[start->r][start->c] = PATH;
    maze->arr[end->r][end->c] = PATH;
}

void PrintMaze(const MazeType *maze, PosType player, PosType start, PosType end, const Stack *solutionPath)
{
    char **displayMaze = (char **)malloc(maze->m * sizeof(char *));
    for (int i = 0; i < maze->m; i++)
    {
        displayMaze[i] = (char *)malloc(maze->n * sizeof(char));
        memcpy(displayMaze[i], maze->arr[i], maze->n * sizeof(char));
    }

    if (solutionPath && !StackEmpty(*solutionPath))
    {
        LinkType p = solutionPath->top;
        while (p)
        {
            PosType pos = p->data.seat;
            if (!Same(pos, start) && !Same(pos, end))
            {
                displayMaze[pos.r][pos.c] = VISITED;
            }
            p = p->next;
        }
    }

    displayMaze[start.r][start.c] = START;
    displayMaze[end.r][end.c] = END;
    if (player.r >= 0 && player.c >= 0)
    {
        displayMaze[player.r][player.c] = PLAYER;
    }

    for (int i = 0; i < maze->m; i++)
    {
        for (int j = 0; j < maze->n; j++)
        {
            printf("%c ", displayMaze[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < maze->m; i++)
    {
        free(displayMaze[i]);
    }
    free(displayMaze);
}

static bool Pass(MazeType *maze, PosType pos)
{
    return maze->arr[pos.r][pos.c] == PATH;
}

static void FootPrint(MazeType *maze, PosType pos)
{
    maze->arr[pos.r][pos.c] = VISITED;
}

static void MarkPrint(MazeType *maze, PosType pos)
{
    maze->arr[pos.r][pos.c] = DEAD_END;
}

PosType NextPos(PosType pos, int di)
{
    PosType next = pos;
    switch (di)
    {
    case 1:
        next.c++;
        break;
    case 2:
        next.r++;
        break;
    case 3:
        next.c--;
        break;
    case 4:
        next.r--;
        break;
    }
    return next;
}

bool MazePath(MazeType *maze, PosType start, PosType end, Stack *S)
{
    InitStack(S);
    PosType curpos = start;
    int curstep = 1;
    bool found = false;
    ElemType e;

    do
    {
        if (Pass(maze, curpos))
        {
            FootPrint(maze, curpos);
            e.step = curstep;
            e.seat = curpos;
            e.di = 1;
            Push(S, e);

            if (Same(curpos, end))
            {
                found = true;
            }
            else
            {
                curpos = NextPos(curpos, 1);
                curstep++;
            }
        }
        else
        {
            if (!StackEmpty(*S))
            {
                Pop(S, &e);
                curstep--;

                while (e.di == 4 && !StackEmpty(*S))
                {
                    MarkPrint(maze, e.seat);
                    Pop(S, &e);
                    curstep--;
                }

                if (e.di < 4)
                {
                    e.di++;
                    Push(S, e);
                    curstep++;
                    curpos = NextPos(e.seat, e.di);
                }
            }
        }
    } while (!StackEmpty(*S) && !found);

    return found;
}
