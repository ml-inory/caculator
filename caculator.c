//
// Created by 祈Inory on 2022/11/12.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 布尔类型
typedef int     BOOL;
#define TRUE    1
#define FALSE   0

// 运算符数量
#define OPERATOR_NUM    6
// 运算符
const char OPERATORS[OPERATOR_NUM] = {
        '+',
        '-',
        '*',
        '/',
        '(',
        ')'
};

// 读入的字符串最大长度
#define LINE_LENGTH     256
// 读入的一行字符串
static char LINE[LINE_LENGTH] = {0};
// 字符串真实长度
static int REAL_LEN = 0;
// 分析的索引
static int INDEX = 0;
// 栈最大长度
#define STACK_SIZE  256
// 操作数栈
static int STACK[STACK_SIZE] = {0};
// 表达式：开始
static int STACK_START = 0;
// 表达式：结束
static int STACK_END = 0;
// 语法树长度
#define AST_SIZE    256
// 语法树
static int AST[AST_SIZE] = {0};


static BOOL IsNumber(char c) {
    if (c >= '0' && c <= '9')
        return TRUE;
    else
        return FALSE;
}

static BOOL IsOperator(char c) {
    int i = 0;
    for (; i < OPERATOR_NUM; ++i) {
        if (c == OPERATORS[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL IsValid(char c) {
    if (IsNumber(c) || IsOperator(c))
        return TRUE;
    else
        return FALSE;
}

static BOOL CheckChar(char c) {
    if (IsValid(c)) {
//        printf("%c\n", c);
        return TRUE;
    }
    else {
        printf("Unknown character: %c\n", c);
        return FALSE;
    }
}

static BOOL GetNumber(int* pNum) {
    int sign = 1;
    if (LINE[INDEX] == '-') {
        sign = -1;
        INDEX++;
    }

    int i = INDEX;
    int j = 0;
    char NumStr[LINE_LENGTH];
    memset(NumStr, 0, LINE_LENGTH);
    for (; i < REAL_LEN; ++i) {
        if (IsNumber(LINE[i])) {
            NumStr[j++] = LINE[i];
            INDEX++;
        } else {
            break;
        }
    }
    if (strlen(NumStr) == 0)
        return FALSE;

    *pNum = atoi(NumStr) * sign;
    return TRUE;
}

// 获取二目操作符
static BOOL GetBinOp(char* pOp) {
    char c = LINE[INDEX++];
    if (c == '+' ||
        c == '-' ||
        c == '*' ||
        c == '/') {
        *pOp = c;
        return TRUE;
    } else {
        return FALSE;
    }
}

// 获取运算符优先级
static int GetOpLevel(char c) {
    switch (c) {
        case '+':
        case '-':
            return 0;
        case '*':
        case '/':
            return 1;
    }
}

// 操作数入栈
static void PushOperand(int num) {
    STACK[STACK_END++] = num;
}

// 运算符入栈
static void PushOperator(char c) {
    STACK[STACK_END++] = c;
}

// 打印语法树
static void PrintAST() {
    int j = 0;
    printf("AST size %d: ", STACK_END - STACK_START);
    for (j = 0; j < STACK_END - STACK_START; ++j) {
        if (j % 2 == 0 || j == 0) {
            if (j != STACK_END - STACK_START - 1)
                printf("(%d)%d ", j, AST[j]);
            else
                printf("%d ", AST[j]);
        }
        else
            printf("%d ", AST[j]);
    }
    printf("\n");
}

// 构造语法树
static void ConstructAST() {
    int i = 0;
    int j = 0;
    memset(AST, 0, AST_SIZE);
    for (j = STACK_START; j < STACK_END; ++j) {
        AST[i++] = STACK[j];
    }
    // 变成完全二叉树
    for (j = 1; j < STACK_END - STACK_START; j+=2) {
//        printf("Swap %c and %d\n", AST[j], AST[j-1]);
        int tmp = AST[j];
        AST[j] = AST[j-1];
        AST[j-1] = tmp;
    }
    // 根据运算符优先级调整顺序
    int size = STACK_END - STACK_START;
    i = 0;
    while (i < size - 3) {
        if (i + 2 < size && GetOpLevel(AST[i]) > GetOpLevel(AST[i+2])) {
            int lhs = AST[i+1];
            int rhs = AST[i+3];
            char op = AST[i];
            int result = 0;
            if (op == '*') {
                result = lhs * rhs;
            } else if (op == '/') {
                result = lhs / rhs;
            }
            AST[i] = AST[i+2];
            AST[i+1] = result;
            memcpy(AST+i+2, AST+i+4, sizeof(int)*(STACK_END-i-4));
            size -= 2;
            STACK_END -= 2;
        }
        i+=2;
    }

//    PrintAST();
}

// 通过AST计算表达式的值
static int ComputeAST() {
    int i = 0;
    for (i = STACK_END - STACK_START - 3; i >= 0; i-=2) {
        switch (AST[i]) {
            case '+':
//                printf("%d + %d\n", AST[i+1], AST[i+2]);
                AST[i] = AST[i+1] + AST[i+2];
                break;
            case '-':
//                printf("%d - %d\n", AST[i+1], AST[i+2]);
                AST[i] = AST[i+1] - AST[i+2];
                break;
            case '*':
//                printf("%d * %d\n", AST[i+1], AST[i+2]);
                AST[i] = AST[i+1] * AST[i+2];
                break;
            case '/':
//                printf("%d / %d\n", AST[i+1], AST[i+2]);
                AST[i] = AST[i+1] / AST[i+2];
                break;
        }
//        PrintAST();
    }
    return AST[0];
}

// 计算一个没有括号的表达式的值
static int ComputeExp() {
    if (STACK_END - STACK_START == 1) {
        return STACK[STACK_END - 1];
    } else {
        ConstructAST();
        return ComputeAST();
    }
}

// 处理一次输入
static void Scan() {
    int i = 0;
    int result = 0;
    BOOL expect_num = TRUE;
    INDEX = 0;
    printf(">");
    memset(LINE, 0, LINE_LENGTH);
    gets_s(LINE, LINE_LENGTH - 1);
    REAL_LEN = strlen(LINE);

    while (INDEX < REAL_LEN) {
        char c = LINE[INDEX];
        if (' ' == c) {
            INDEX++;
            continue;
        }

        if (TRUE != CheckChar(c))
            break;

        int num;
        if (expect_num) {
            if (GetNumber(&num)) {
                PushOperand(num);
            } else {
                printf("Syntax error!\n");
                printf("%s\n", LINE);
                for (i = 0; i < INDEX - 1; ++i) {
                    printf(" ");
                }
                printf("^\n");
                return;
            }
            expect_num = FALSE;
        }
        else {
            if (0 == c) {
                break;
            }
//            printf("%c\n", LINE[INDEX]);
            if ('(' == c) {
                expect_num = TRUE;
                STACK_START = STACK_END;
            } else if (')' == c) {
                ComputeExp();
            } else {
                PushOperator(c);
                expect_num = TRUE;
            }

            INDEX++;
        }
    }

    if (STACK_START < STACK_END) {
        result = ComputeExp();
        printf("%d\n", result);
        STACK_START = STACK_END = 0;
    }
}

int main() {
    while (1)
        Scan();

    return 0;
}