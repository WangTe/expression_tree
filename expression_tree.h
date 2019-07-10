//
// Created by Once on 2019/7/9.
//

#ifndef DATALGORITHM_MATHEMATICA_H
#define DATALGORITHM_MATHEMATICA_H

/**
 * 表达式：-30.ax^2.5+3x - 5b
 * 表达式规则：a^0-64.a.b4
 * 1、运算符仅限于+ - * / ^ ( )，（字符还包括小数点.）
 * 2、代数式只能使用数字和字母，（字符、数字、运算符、小数点、空格）
 * 3、数字可使用正负数、整数和浮点数，字母只可使用"单字符"作为标识符，空格分隔表示相乘，
 * 例如ab表现和a b表示a*b，数字和字符相连表示相乘，如3a/a3=3*a，数字和数字空格表示相乘3 5=3*5
 * */

/**
 * 词法分析: 字符流转为记号流（token）
 * 语法分析：输出抽象语法树
 * 处理流程：
 * 1、语法规则：
 *      非法字符
 *      全为字符、数字、运算符、小数点、空格，或组合，错误
 *      ()配对
 *      小数点.配对：数字和小数点.的组合中只能有一个小数点，以小数点开头表示纯小数，如.5表示0.5，数字后面的小数点属于数字本身
 *      字母和字母间的小数点，错误
 *      连续重复的运算符，错误
 *      尾部运算符，错误
 * 2、初步转化：
 *      将字母和字母、字母和数字、数字和数字之间的空格用*替换；
 *      去除所有空格
 * 3、转换成后缀表达式
 * 4、生成表达式树：
 *      先分词，转换成一个个token
 * */

#define TOKEN_MAX 16

// 单项式
typedef struct monomial{
    int priority; // 数字的优先级为-1，字母为0，()为20，+-为13，*/为14，^为15
    char token[TOKEN_MAX];
} Monomial;

// 表达式树结点
typedef struct enode{
    Monomial monomial;
    struct enode *left;
    struct enode *right;
} ENode;

typedef int Cell;
typedef struct cnode{
    Cell cell;
    struct cnode *next;
    struct cnode *prev;
} CNode;

// 表达式树ADT
typedef struct exp{
    ENode *root;
    unsigned int size;
} Exp;

// 栈ADT
typedef struct cstack{
    CNode *head;
    CNode *tail;
    unsigned int size;
} CStack;

// 算法操作声明
extern CStack *cstack_init();
extern int cstack_push(CStack *cstack, Cell cell);
extern Cell cstack_pop(CStack *cstack);
extern Cell cstack_peek(CStack *cstack);
extern int cstack_clear(CStack *cstack);

extern Exp *exp_init();
extern int exp_load(Exp *exp, char str[]);
//extern int exp_add(Exp *exp, Monomial *monomial);
extern void exp_traverse(Exp *exp);
extern int exp_clear(Exp *exp);

#endif //DATALGORITHM_MATHEMATICA_H
