
#ifndef PARSER
#define PARSER
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "lexer.h"

// Forward declarations
typedef struct Expression Expression;
struct Statement;
typedef struct ExpressionArray ExpressionArray;
typedef struct StatementArray StatementArray;

// Enum for binary operation types
enum BinaryOpType {
    Eq,
    St,
    Gt,
    Neq
};

typedef struct ExpressionArray {
    Expression* array;
    size_t size;
} ExpressionArray;

typedef struct StatementArray {
    struct Statement* array;
    size_t size;
} StatementArray;

enum ExpressionType {
    ExprConstNum,
    ExprConstBool,
    ExprConstStr,
    ExprList,
    ExprLambda,
    ExprConstIndexing,
    ExprArrayPush,
    ExprIndexing,
    ExprAdd,
    ExprSub,
    ExprMul,
    ExprDiv,
    ExprBinaryOp,
    ExprCall,
    ExprLoadVar
};

typedef union ExpressionUnion {
    int64_t const_num;
    bool const_bool;
    const char *const_str;
    struct ExpressionArray list; 
    struct {
        struct StatementArray body;
        Expression *args;
        size_t num_args;
    } lambda;
    struct {
        Expression *parent;
        size_t index;
    } const_indexing;
        struct {
        Expression *first;
        Expression *second;
    } math;
    struct {
        Expression *parent;
        Expression *value;
    } array_push;
    struct {
        Expression *parent;
        Expression *index;
    } indexing;
    struct {
        Expression *left;
        Expression *right;
    } binary_op;
    struct {
        char *name;
        ExpressionArray args;
    } call;
    char *load_var;
} ExpressionUnion;

// Expression struct definition
typedef struct Expression {
    enum ExpressionType expr_type;
    union ExpressionUnion data;
} Expression;

enum StatementType {
    StmtExpr,
    StmtFor,
    StmtArrayAssign,
    StmtArrayAssignConst,
    StmtAssign,
    StmtIf,
    StmtWhile
};

typedef union StatementUnion {
    struct Expression *expr;
    struct {
        char *name;
        struct Expression *iterable;
        struct StatementArray body;
    } for_stmt;
    struct {
        struct Expression array;
        struct Expression index;
        struct Expression value;
    } array_assign;
    struct {
        struct Expression array;
        size_t index;
        struct Expression value;
    } array_assign_const;
    struct {
        char *name;
        struct Expression expr;
    } assign;
    struct {
        struct Expression condition;
        struct StatementArray body;
        struct StatementArray else_body;
    } if_stmt;
    struct {
        struct Expression condition;
        StatementArray body;
    } while_stmt;
} StatementUnion;

typedef struct Statement {
    enum StatementType type;
    union StatementUnion data;
} Statement;

StatementArray parse(TokenArray tokens);

void format_expr(Expression* expression);
void format_stmt(Statement* stmt);


#endif  