#include <string.h>
#include "stack.h"
#include "queue.h"
#include "translate.h"


typedef struct
{
    char lhs;
    const char *rhs;
} Rule;

/* 产生式规则 */
static Rule g_rules[] = {
    {'A', "sae"},
    {'B', "tAdA"},
    {'M', "魔"},
    {'W', "王"},
    {'Y', "语"},
    {'X', "言"},
    {'F', "翻"},
    {'C', "译"},
    {'Q', "器"}
};
static const int RULE_COUNT = sizeof(g_rules) / sizeof(g_rules[0]);

/* 查找产生式 */
static const char *find_rule(char lhs)
{
    for (int i = 0; i < RULE_COUNT; ++i)
    {
        if (g_rules[i].lhs == lhs)
            return g_rules[i].rhs;
    }
    return NULL;
}

/* 括号特殊规则 */
typedef struct
{
    const char *inside;      /* 括号内字符串 */
    const char *replacement; /* 替换 */
} BracketRule;

static BracketRule g_bracket_rules[] = {
    {"ehnxgz", "ezegexenehe"}};
static const int BRACKET_RULE_COUNT =
    sizeof(g_bracket_rules) / sizeof(g_bracket_rules[0]);

/* 根据括号内部内容查找规则 */
static const char *find_bracket_rule(const char *inside)
{
    for (int i = 0; i < BRACKET_RULE_COUNT; ++i)
    {
        if (strcmp(inside, g_bracket_rules[i].inside) == 0)
            return g_bracket_rules[i].replacement;
    }
    return NULL;
}

void translate(const char *magic, char *human, int humanSize)
{
    SqStack chStack;    /* 处理用的字符栈 */
    SqStack transStack; /* 结果栈：存放翻译后的小写字母 */
    SqQueue chQueue;    /* 括号处理用队列 */

    InitStack(&chStack);
    InitStack(&transStack);
    InitQueue(&chQueue);

    /* 进栈 */
    for (int i = 0; magic[i] != '\0'; ++i)
    {
        Push(&chStack, magic[i]);
    }

    SElemType e;
    char buffer[256];

    while (!StackEmpty(&chStack))
    {
        Pop(&chStack, &e);

        const char *rhs = find_rule(e);
        if (rhs != NULL)
        {
            /* 按产生式规则替换 */
            for (int i = 0; rhs[i] != '\0'; ++i)
            {
                Push(&chStack, rhs[i]);
            }
        }
        else if (e == ')')
        {
            /* 处理括号 */
            ClearQueue(&chQueue);
            SElemType c;
            int len = 0;

            while (!StackEmpty(&chStack))
            {
                Pop(&chStack, &c);
                if (c == '(')
                    break;
                EnQueue(&chQueue, c);
            }

            /* 得到原始顺序 */
            len = 0;
            while (!QueueEmpty(&chQueue) && len < (int)sizeof(buffer) - 1)
            {
                DeQueue(&chQueue, &c);
                buffer[len++] = c;
            }
            buffer[len] = '\0';
            for (int i = 0; i < len / 2; ++i)
            {
                char tmp = buffer[i];
                buffer[i] = buffer[len - 1 - i];
                buffer[len - 1 - i] = tmp;
            }

            /* 按括号规则替换 */
            const char *rep = find_bracket_rule(buffer);
            if (rep != NULL)
            {
                for (int i = 0; rep[i] != '\0'; ++i)
                {
                    Push(&chStack, rep[i]);
                }
            }
            else
            {
                /* 找不到规则原样压回去 */
                for (int i = 0; buffer[i] != '\0'; ++i)
                {
                    Push(&chStack, buffer[i]);
                }
            }
        }
        else
        {
            Push(&transStack, e);
        }
    }

    /* 将结果栈中的字符依次弹出，得到人类语言 */
    int pos = 0;
    while (!StackEmpty(&transStack) && pos < humanSize - 1)
    {
        Pop(&transStack, &e);
        human[pos++] = e;
    }
    human[pos] = '\0';

    DestroyStack(&chStack);
    DestroyStack(&transStack);
    DestroyQueue(&chQueue);
}
