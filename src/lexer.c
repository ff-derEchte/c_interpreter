#include "lexer.h"
#include "arraylist.h"
#include<string.h>
#include<stdbool.h>

Token* create_token_ptr(TokenType type, char* value) {
    Token* token = malloc(sizeof(Token));

    token->token_type = type;
    token->value = value;

    return token;
}

TokenArray create_array_from_list(ArrayList* list) {
    Token* tokens = malloc(list->len*sizeof(Token));

    for(size_t i = 0; i < list->len; i++) {
        tokens[i] = *(Token*)list->data[i];
    }

    TokenArray arr;
    arr.data = tokens;
    arr.size = list->len;
    return arr;
}

bool isNumber(const char *str) {
    if (*str == '\0') {
        return false;
    }

    while (*str != '\0') {
        if (*str < '0' || *str > '9') {
            return false; 
        }
        str++;
    }

    return true;
}

TokenArray lex_code(const char* code) {

    ArrayList list = al_create(20);

    char buffer[50];
    char* buffer_ptr = buffer;

    while(*code != '\0') {
        //printf("%c\n", *code);
        switch (*code) {
            case '+':
            case '-':
            case '*':
            case '/':
                al_add_item(&list, create_token_ptr(Math, code));
                break;
            case '=':
                al_add_item(&list, create_token_ptr(Assign, code));
                break;
            case '(':
                al_add_item(&list, create_token_ptr(OpeningBracket, code));
                break;
            case ')':
                al_add_item(&list, create_token_ptr(ClosingBracket, code));
                break;
            case '{':
                al_add_item(&list, create_token_ptr(CurlyOpen, code));
                break;
            case '}':
                al_add_item(&list, create_token_ptr(CurlyClose, code));
                break;
            case ',':
                al_add_item(&list, create_token_ptr(Comma, code));
                break;
            case ' ':
                size_t length = buffer_ptr - buffer;

                if (length == 0) {
                    break;
                }
                //allocate +1 for missing null char
                char* word_ptr = malloc(length+1);

                memcpy(word_ptr, buffer, length);

                //set last item to \0 as the buffer doesnt have a null char
                word_ptr[length] = '\0';

                if (isNumber(buffer)) {
                    al_add_item(&list, create_token_ptr(Number, word_ptr));
                } else if (strcmp(buffer, "true") == 0) {
                    al_add_item(&list, create_token_ptr(BoolTrue, word_ptr));
                } else if (strcmp(buffer, "false") == 0) {
                    al_add_item(&list, create_token_ptr(BoolFalse, word_ptr));
                } else if (strcmp(buffer, "if") == 0) {
                    al_add_item(&list, create_token_ptr(If, word_ptr));
                } else if (strcmp(buffer, "while") == 0) {
                    al_add_item(&list, create_token_ptr(While, word_ptr));
                } else {
                    al_add_item(&list, create_token_ptr(Identifier, word_ptr));
                }

                buffer_ptr = buffer;
                break;
            default:
                *buffer_ptr = *code;
                buffer_ptr++;
                break;
        }
        code++;
    }

    TokenArray array = create_array_from_list(&list);
    al_delete(&list);
    return array;
}