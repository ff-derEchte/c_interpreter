#ifndef LEXER
#define LEXER

#include<stdlib.h>

typedef enum {
    Identifier,
    Math,
    Number,
    BoolTrue,
    BoolFalse,
    Assign,
    OpeningBracket,
    ClosingBracket,
    Comma,
    While,
    If,
    Else,
    CurlyOpen,
    CurlyClose
} TokenType;

typedef struct {
    TokenType token_type;
    char* value;
} Token;

typedef struct {
    Token* data;
    size_t size;
} TokenArray;

TokenArray lex_code(const char* code);

#endif