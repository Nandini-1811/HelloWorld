#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ir.h"

/* Runtime value types */
typedef enum{
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING,
    VAL_BOOL,
    VAL_NOTHING
} ValueKind;

/* A runtime value */
typedef struct{
    ValueKind kind;
    union{
        int int_val;
        double float_val;
        char* str_val;
        int bool_val;
    } as;
} Value;

/* A runtime variable entry */
typedef struct RuntimeVar{
    char* name;
    Value value;
    struct RuntimeVar* next;
} RuntimeVar;

/* One scope / one function frame */
typedef struct Frame{
    RuntimeVar** vars;
    int var_size;
    struct Frame* prev;
} Frame;

/* The interpreter state */
typedef struct{
    Frame* current_frame;     /* active scope */

    Value* temps;             /* t0, t1, t2... */
    int temp_size;

    Value* call_stack;        /* for arguments */
    int call_top;
    int call_size;

    Value return_val;
    int returning;
} Interpreter;

/* Functions */
Interpreter* interp_create(void);
void interp_destroy(Interpreter* interp);
void interp_run(Interpreter* interp, IRProgram* program);

#endif