#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "expression_tree.h"


int main() {
    char str[] = "a^h - 3.14^e + b - (2.5x - 3^c) - 8xy + 3x^2z - yz";
    char str1[] = "a + b * c + (d * e + f) * g";
    Exp *exp = exp_init();
    exp_load(exp, str);
    exp_traverse(exp);
    exp_clear(exp);
    return 0;
}