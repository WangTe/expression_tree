//
// Created by Once on 2019/7/9.
//

#include "expression_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

static int istag(int ch){
    if(ch == '(' || ch == ')' || ch == '.')
        return 1;
    else
        return 0;
}

static int isoperator(int ch){
    if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^')
        return 1;
    else
        return 0;
}

// 粗略词法、语法检测
static int exp_syntax_analyze(char exp[]){
    if(exp == NULL)
        return 0;
    // 尾部运算符
    if(isoperator(exp[strlen(exp) - 1]))
        return 0;
    int status = 0;
    int prev = -1;
    for (int i = 0; i < strlen(exp); ++i) {
        // 非法字符检测
        if(isalnum(exp[i]) || isoperator(exp[i]) || istag(exp[i]) || isspace(exp[i]))
            status = 1;
        else{
            status = 0;
            return status;
        }

        // 连续重复的运算符
        if(exp[i] == prev && isoperator(exp[i]))
            return 0;
        prev = exp[i];

        // 字母和字母、字母和数字间的小数点
        int k = i + 1;
        if(k < strlen(exp) - 1 && exp[k] == '.' && isalpha(exp[k - 1]) && isalnum(exp[k + 1]))
            return 0;
    }

    // ()配对
    CStack *stack = cstack_init();
    for (int j = 0; j < strlen(exp); ++j) {
        Cell cell = exp[j];
        if(cstack_peek(stack) == '(' && cell == ')')
            cstack_pop(stack);
        else if(cell == '(' || cell == ')')
            cstack_push(stack, cell);
    }
    int size = stack->size;
    cstack_clear(stack);
    if(size != 0)
        return 0;

    // 部分或全部为字符、数字、运算符、小数点、空格（略）
    return status;
}

// 标准化表达式
static char *exp_normalize(char exp[]){
    if(exp == NULL)
        return 0;
    if(!exp_syntax_analyze(exp))
        return NULL;
    char *result = (char*)malloc(sizeof(char) * 2 * strlen(exp));
    char temp[strlen(exp)];
    int size = 0;
    for (int i = 0; i < strlen(exp); ++i) {
        // 将字母和字母、字母和数字、数字和数字之间的空格用*替换
        if(i > 0 && i < strlen(exp) - 1 && isspace(exp[i]) && isalnum(exp[i - 1]) && isalnum(exp[i + 1]))
            exp[i] = '*';
        // 去除整数后的小数点5.a=>5a
        if(i > 0 && i < strlen(exp) - 1 && exp[i] == '.' && isdigit(exp[i - 1]) && isalpha(exp[i + 1]))
            continue;
        // 去除所有空格
        if(isspace(exp[i]))
            continue;
        temp[size] = exp[i];
        size++;
    }
    // 字母和数字、数字和字母、字母和字母之间添加*
    int k = 0;
    for (int j = 0; j < size; ++j) {
        result[k] = temp[j];
        if(j < size - 1 && ((isalpha(temp[j]) && isdigit(temp[j + 1])) || (isdigit(temp[j]) && isalpha(temp[j + 1]))
        || (isalpha(temp[j]) && isalpha(temp[j + 1])))){
            k++;
            result[k] = '*';
        }
        k++;
    }
    result[k] = '\0';
    return result;
}

// 中缀转为后缀表达式
static char *exp_to_postfix(char exp[]){
    if(exp == NULL)
        return NULL;
    char *str = exp_normalize(exp);
    printf("%s\n", str);
    if(!str)
        return NULL;
    char *temp = (char*)malloc(strlen(str));
    CStack *stack = cstack_init();
    int size = 0, count = 0;
    for (int i = 0; i < strlen(str); ++i) {
        if(str[i] == '('){
            count++;
            cstack_push(stack, str[i]);
            continue;
        }
        if(str[i] == ')' && count > 0){
            count--;
            Cell cell = cstack_pop(stack);
            while(cell != '('){
                temp[size] = cell;
                cell = cstack_pop(stack);
                size++;
            }
            continue;
        }
        if(isoperator(str[i])){
            // 遇到栈比当前运算符优先级高，则弹出
            if(((str[i] == '+' || str[i] == '-') && (cstack_peek(stack) == '*' || cstack_peek(stack) == '/' || cstack_peek(stack) == '^')) ||
                    ((str[i] == '*' || str[i] == '/') && (cstack_peek(stack) == '^'))){
                Cell cell = cstack_pop(stack);
                // 一直弹出，直到遇到栈中的小于或等于当前运算符优先级
                while(cell != -1){
                    temp[size] = cell;
                    Cell c = cstack_peek(stack);
                    if(c == '('){
                        size++;
                        break;
                    }
                    if(((str[i] == '+' || str[i] == '-') && (c == '+' || c == '-')) ||
                            ((str[i] == '*' || str[i] == '/') && (c == '+' || c == '-' || c == '*' || c == '/')) ||
                            (str[i] == '^' && c == '^')){
                        size++;
                        temp[size] = cstack_pop(stack);
                        size++;
                        break;
                    }
                    cell = cstack_pop(stack);
                    size++;
                }
            }
            cstack_push(stack, str[i]);
        }
        else{
            temp[size] = str[i];
            size++;
        }
    }
    if(stack->size > 0){
        Cell cell = cstack_pop(stack);
        while(cell != -1){
            temp[size] = cell;
            cell = cstack_pop(stack);
            size++;
        }
    }
    cstack_clear(stack);

    for (int j = 0; j < size; ++j) {
        str[j] = temp[j];
    }
    str[size] = '\0';
    return str;
}

// 后缀表达式转为表达式树
static int exp_to_tree(Exp *exp, char str[]){
    if(exp == NULL || str == NULL)
        return 0;
    char *postfix = exp_to_postfix(str);
    printf("%s\n", postfix);
    if(!postfix)
        return 0;
    CStack *stack = cstack_init();
    for (int i = 0; i < strlen(postfix); ++i) {
        if(isoperator(postfix[i])){
            // 数字优先级为-1，字母为0，()为20，+-为13，*/为14，^为15
            ENode *node = (ENode*)malloc(sizeof(ENode));
            if(!node){
                perror("init node error.");
                return 0;
            }
            node->left = node->right = NULL;
            if(postfix[i] == '+' || postfix[i] == '-')
                node->monomial.priority = 13;
            else if(postfix[i] == '*' || postfix[i] == '/')
                node->monomial.priority = 14;
            else if(postfix[i] == '^')
                node->monomial.priority = 15;
            else
                node->monomial.priority = -1;
            node->monomial.token[0] = postfix[i];
            node->monomial.token[1] = '\0';

            if(stack->size > 0){
                node->right = (ENode*)cstack_pop(stack);
                if(stack->size > 0)
                    node->left = (ENode*)cstack_pop(stack);
            }
            cstack_push(stack, (Cell)node);
            exp->size++;
        }
        else{
            if(isalpha(postfix[i])){
                ENode *node = (ENode*)malloc(sizeof(ENode));
                if(!node){
                    perror("init node error.");
                    return 0;
                }
                node->left = node->right = NULL;
                node->monomial.priority = 0;
                node->monomial.token[0] = postfix[i];
                node->monomial.token[1] = '\0';
                cstack_push(stack, (Cell)node);
                exp->size++;
            }
            else if(isdigit(postfix[i])){
                ENode *node = (ENode*)malloc(sizeof(ENode));
                if(!node){
                    perror("init node error.");
                    return 0;
                }
                node->left = node->right = NULL;
                node->monomial.priority = -1;
                int size = 0;
                for (; i < strlen(postfix) && size < TOKEN_MAX; ++i) {
                    if(isalpha(postfix[i]) || isoperator(postfix[i])){
                        i--;
                        break;
                    }
                    node->monomial.token[size] = postfix[i];
                    size++;
                }
                node->monomial.token[size] = '\0';
                cstack_push(stack, (Cell)node);
                exp->size++;
            }
        }
    }
    printf("size: %d, %d\n", exp->size, stack->size);
    exp->root = (ENode*)cstack_pop(stack);
    cstack_clear(stack);
    free(postfix);
    return 1;
}

CStack *cstack_init(){
    CStack *stack = (CStack*)malloc(sizeof(CStack));
    if(!stack){
        perror("init stack error.");
        return NULL;
    }
    stack->size = 0;
    stack->head = stack->tail = NULL;
    return stack;
}

int cstack_push(CStack *cstack, Cell cell){
    if(cstack == NULL)
        return 0;
    CNode *node = (CNode*)malloc(sizeof(CNode));
    if(!node)
        return 0;
    node->cell = cell;
    node->next = node->prev = NULL;
    if(cstack->size == 0){
        cstack->head = cstack->tail = node;
        cstack->size++;
        return 1;
    }
    node->prev = cstack->tail;
    cstack->tail->next = node;
    cstack->tail = node;
    cstack->size++;
    return 1;
}

Cell cstack_pop(CStack *cstack){
    if(cstack == NULL || cstack->size == 0)
        return -1;
    CNode *node = cstack->tail;
    if(cstack->head == node){
        cstack->head = cstack->tail =  NULL;
    }
    else{
        node->prev->next = NULL;
        cstack->tail = node->prev;
    }
    Cell cell = node->cell;
    free(node);
    cstack->size--;
    return cell;
}

Cell cstack_peek(CStack *cstack){
    if(cstack == NULL || cstack->size == 0)
        return -1;
    return cstack->tail->cell;
}

int cstack_clear(CStack *cstack){
    if(cstack == NULL)
        return 0;
    CNode *node = cstack->head;
    while(node){
        CNode *temp = node->next;
        free(node);
        node = temp;
    }
    free(cstack);
    return 1;
}


Exp *exp_init(){
    Exp *exp = (Exp*)malloc(sizeof(Exp));
    if(!exp){
        perror("init exp error");
        return NULL;
    }
    exp->root = NULL;
    exp->size = 0;
    return exp;
}

int exp_load(Exp *exp, char str[]){
    if(!exp_to_tree(exp, str))
        return 0;
    return 1;
}

// 使用后缀遍历得到后缀表达式
static void traverse(ENode *root){
    if(!root)
        return;
    traverse(root->left);
    traverse(root->right);
    printf("%s", root->monomial.token);
}

void exp_traverse(Exp *exp){
    if(exp == NULL || exp->size == 0)
        return;
    traverse(exp->root);
}

static int clear(ENode *root){
    if(!root)
        return 0;
    clear(root->left);
    clear(root->right);
    free(root);
    return 1;
}

int exp_clear(Exp *exp){
    if(exp == NULL)
        return 0;
    if(exp->size != 0)
        clear(exp->root);
    free(exp);
    return 1;
}
