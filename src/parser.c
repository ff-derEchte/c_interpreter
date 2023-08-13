#include "parser.h"
#include "lexer.h"
#include "arraylist.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    Token* pointer;
    Token* begin;
    size_t length;
} Navigator;

Expression parse_expression(Navigator* nav);
Statement parse_statement(Navigator* nav);
Expression parse_expression_base(Navigator* nav);
Expression parse_expression_mul(Navigator* nav);
StatementArray parse_statements(Navigator* nav);

Navigator create_navigator(TokenArray array) {
    Navigator nav;
    nav.begin = array.data;
    nav.pointer = array.data;
    nav.length = array.size;
    return nav;
}

Token* next(Navigator* nav) {
    nav->pointer++;

    //check if out of bounds
    if (nav->pointer - nav->begin >= nav->length) {
        return NULL;
    }

    return nav->pointer;
}

Token* visit_next(Navigator* nav) {
    if (nav->pointer - nav->begin + 1 >= nav->length) {
        return NULL;
    }

    return nav->pointer+1;
}

Expression create_expression(enum ExpressionType type, union ExpressionUnion data) {
    Expression expr;
    expr.expr_type = type;
    expr.data = data;

    return expr;
}

Statement create_statement(enum StatementType type, union StatementUnion data) {
    Statement stmt;
    stmt.type = type;
    stmt.data = data;
    
    return stmt;
}

Expression* alloc_expression(const Expression expr) {
    Expression* expr_ptr = malloc(sizeof(Expression));
    *expr_ptr = expr;
    return expr_ptr;
}

StatementArray parse(TokenArray tokens) {
    Navigator nav = create_navigator(tokens);
    return parse_statements(&nav);
}

StatementArray parse_statements(Navigator* nav) {
    size_t cap = 20;
    Statement* statements = malloc(sizeof(Statement)*cap);
    Statement* current = statements;

    while(visit_next(nav)->token_type != CurlyClose) {
        *current = parse_statement(nav);
        current++;

        if (current - statements >= cap) {
            size_t new_cap = (3*cap)/2;
            Statement* new_statements = malloc(sizeof(Statement)*new_cap);
            memcpy(new_statements, statements, cap);

            //set the current pointer to the correct position
            current = new_statements+(current-statements);

            cap = new_cap;
            statements = new_statements;
        }
    }

    next(nav); //the closing curly

    return (StatementArray){.array = statements, .size = (current-statements)};
}

Statement parse_statement(Navigator* nav) {
    Token* word = next(nav);
    switch (word->token_type)
    {
    case While:
        Expression condition = parse_expression(nav);

        if (next(nav)->token_type != CurlyOpen) {
            printf("Expeced Opening curly brace");
            exit(1);
        }

        StatementArray body = parse_statements(nav);

        return create_statement(StmtWhile, (StatementUnion) {
            .while_stmt = {
                .condition = condition,
                .body = body
            }
        });
    case If:
        Expression while_condition = parse_expression(nav);

        if (next(nav)->token_type != CurlyOpen) {
            printf("Expeced Opening curly brace");
            exit(1);
        }

        StatementArray while_body = parse_statements(nav);
        
        if (visit_next(nav)->token_type != Else) {
            return create_statement(StmtIf, (StatementUnion) {
                .if_stmt = {
                    .body = while_body,
                    .condition = while_condition,
                    .else_body = {
                        .array = malloc(0),
                        .size = 0
                    }
                }
            });
        }

        next(nav); //else
        if (next(nav)->token_type != CurlyOpen) {
            printf("Expeced Opening curly brace");
            exit(1);
        }

        StatementArray else_body = parse_statements(nav);

        return create_statement(StmtIf, (StatementUnion) {
            .if_stmt = {
                .body = body,
                .else_body = else_body,
                .condition = condition
            }
        });
    
    default:
        if (word->token_type == Identifier && visit_next(nav)->token_type == Assign) {
            next(nav); // the equals sign
            Expression value = parse_expression(nav);
            return create_statement(StmtAssign, (StatementUnion) {
                .assign = {
                    .name = word->value,
                    .expr = value
                }
            });
        }

        nav->pointer--; //reset the word to parse it in parse_expression
        Expression* ptr = malloc(sizeof(Expression));
        *ptr = parse_expression(nav);
        return create_statement(StmtExpr, (StatementUnion){.expr = ptr});
    }
}

ExpressionArray parser_calling_args(Navigator* nav) {
    size_t cap = 10;
    Expression* expressions = malloc(sizeof(Expression)*cap);
    Expression* current = expressions;

    while(true) {
        *current = parse_expression(nav);
        current++;

        Token* word = next(nav);
        switch (word->token_type) {
            case Comma:
                continue;
            case ClosingBracket:
                return (ExpressionArray) {.array = expressions, .size = (current-expressions)};
            default:
                printf("Expected , or ) but got %s", word->value);
                exit(1);
        }
    }
}

Expression parse_expression(Navigator* nav) {
    Expression base = parse_expression_mul(nav);

    if (visit_next(nav)->token_type != Math) {
        return base;
    }

    char op = *next(nav)->value;

    if (op != '+' && op != '-') {
        nav->pointer--; //go one step back
        return base;
    }

    Expression second = parse_expression(nav);

    enum ExpressionType tp;

    switch (op) {
        case '+':
            tp = ExprAdd;
            break;
        case '-':
            tp = ExprSub;
            break;
        default:
            printf("internal error");
            exit(2);
    }

    return create_expression(tp, (ExpressionUnion) {
        .math = {
            .first = alloc_expression(base),
            .second = alloc_expression(second)
        }
    });
}

Expression parse_expression_mul(Navigator* nav) {
    Expression base = parse_expression_base(nav);

    if (visit_next(nav)->token_type != Math) {
        return base;
    }

    char op = *next(nav)->value;

    if (op != '*' && op != '/') {
        nav->pointer--; //go one step back
        return base;
    }

    Expression second = parse_expression_mul(nav);

    enum ExpressionType tp;

    switch (op) {
        case '*':
            tp = ExprMul;
            break;
        case '/':
            tp = ExprDiv;
            break;
        default:
            printf("internal error");
            exit(2);
    }

    return create_expression(tp, (ExpressionUnion) {
        .math = {
            .first = alloc_expression(base),
            .second = alloc_expression(second)
        }
    });

}

Expression parse_expression_base(Navigator* nav) {
    Token* word = next(nav);
    switch (word->token_type) {
    case BoolTrue:
        return create_expression(ExprConstBool, (ExpressionUnion){.const_bool = true});
    case BoolFalse:
        return create_expression(ExprConstBool, (ExpressionUnion){.const_bool = true});
    case Number:
        int64_t number = strtol(word->value, NULL, 10);
        return create_expression(ExprConstNum, (ExpressionUnion){.const_num = number});
    case Identifier:

        if (visit_next(nav)->token_type == OpeningBracket) {
            //is function call

            next(nav);  //the '(' char

            return create_expression(ExprCall, (ExpressionUnion) {
                .call = {
                    .name = word->value,
                    .args = parser_calling_args(nav)
                }
            });
        }

        //is loadvar
        return create_expression(ExprLoadVar, (ExpressionUnion){.load_var = word->value});
    default:
        printf("Invalid Token %s\n", word->value);
        exit(1);
        break;
    }
}


void format_expr(Expression* expr) {
    enum ExpressionType tp = expr->expr_type;

    switch (tp) {
        case ExprConstNum:
            printf("ConstNum(%ld)", expr->data.const_num);
            break;
        case ExprConstBool:
            printf("ConstBool(%d)", expr->data.const_bool);
            break;
        case ExprConstStr:
            printf("ConstStr(%s)", expr->data.const_str);
            break;
        case ExprLoadVar:
            printf("LoadVar(%s)", expr->data.load_var);
            break;
        case ExprCall:
            printf("Call(%s, [", expr->data.call.name);
            for (size_t i = 0; i < expr->data.call.args.size; i++) {
                format_expr(&expr->data.call.args.array[i]);
                if ((i+1) < expr->data.call.args.size)
                    printf(", ");
            }
            printf("])");
            break;
        case ExprAdd:
        case ExprSub:
        case ExprMul:
        case ExprDiv:
            switch (tp) {
                case ExprAdd:
                    printf("Add(");
                    break;
                case ExprSub:
                    printf("Sub(");
                    break;
                case ExprMul:
                    printf("Mul(");
                    break;
                case ExprDiv:
                    printf("Div(");
                    break;
            }
            format_expr(expr->data.math.first);
            printf(", ");
            format_expr(expr->data.math.second);
            printf(")");
            break;
        default:
            printf("CannotFormat");
    }
}
void format_stmt(Statement* stmt);

void format_stmts(StatementArray* stmts) {
    printf("{ ");
    for (size_t i = 0; i < stmts->size; i++) {
        format_stmt(&stmts->array[i]);
        if (i+1 < stmts->size) {
            printf(", ");
        }
    }
    printf(" }");
}

void format_stmt(Statement* stmt) {
    enum StatementType tp = stmt->type;

    switch(tp) {
        case StmtExpr:
            printf("Expr(");
            format_expr(stmt->data.expr);
            printf(")");
            break;
        case StmtAssign:
            printf("Assign(%s =", stmt->data.assign.name);
            format_expr(&stmt->data.assign.expr);
            printf(")");
            break;
        case StmtIf:
            printf("If(");
            format_expr(&stmt->data.if_stmt.condition);
            printf(") ");
            format_stmts(&stmt->data.if_stmt.body);
            printf(" else ");
            format_stmts(&stmt->data.if_stmt.else_body);
            break;
        case StmtWhile:
            printf("While(");
            format_expr(&stmt->data.while_stmt.condition);
            printf(") ");
            format_stmts(&stmt->data.while_stmt.body);
            break;
    }
}