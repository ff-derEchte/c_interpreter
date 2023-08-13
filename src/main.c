#include<stdio.h>
#include<stdlib.h>
#include"lexer.h"
#include"parser.h"

int main() {
    TokenArray array = lex_code("{ true + 4 }");
    printf("%ld\n", array.size);
    StatementArray statements = parse(array);
    for (Statement* ptr = statements.array; (ptr-statements.array) < statements.size; ptr++) {
        format_stmt(ptr);
        printf("\n");
    }
}