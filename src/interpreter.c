#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/interpreter.h"
#include "../include/ir.h"

/*--------------------------------------
HelloWorld Interpreter
Stage 5 - Execution Engine

Executes IR Instructions directly.

Walks instruction array , maintains
variable store and temp store.
----------------------------------------*/

#define VAR_TABLE_SIZE 64

/* Hash function for variable names */
static int var_hash(const char *name, int size)
{
    unsigned int h = 0;
    while (*name)
    {
        h = h * 31 + (unsigned char)*name;
        name++;
    }
    return h % size;
}

static Frame *frame_create(Frame *prev)
{
    Frame *f = (Frame *)calloc(1, sizeof(Frame));
    f->var_size = VAR_TABLE_SIZE;
    f->vars = (RuntimeVar **)calloc(VAR_TABLE_SIZE, sizeof(RuntimeVar *));
    f->prev = prev;
    return f;
}

static void frame_destroy(Frame *f)
{
    if (!f)
        return;

    for (int i = 0; i < f->var_size; i++)
    {
        RuntimeVar *v = f->vars[i];
        while (v)
        {
            RuntimeVar *next = v->next;
            free(v->name);
            free(v);
            v = next;
        }
    }

    free(f->vars);
    free(f);
}

/* Create interpreter */
Interpreter *interp_create(void)
{
    Interpreter *interp = (Interpreter *)calloc(1, sizeof(Interpreter));

    interp->current_frame = frame_create(NULL);

    interp->temp_size = 256;
    interp->temps = (Value *)calloc(256, sizeof(Value));

    interp->call_size = 256;
    interp->call_stack = (Value *)calloc(256, sizeof(Value));
    interp->call_top = 0;

    interp->returning = 0;
    interp->return_val.kind = VAL_NOTHING;

    return interp;
}

/* Destroy Interpreter */
void interp_destroy(Interpreter *interp)
{
    while (interp->current_frame)
    {
        Frame *prev = interp->current_frame->prev;
        frame_destroy(interp->current_frame);
        interp->current_frame = prev;
    }

    free(interp->temps);
    free(interp->call_stack);
    free(interp);
}

/* Varaible store */
static void var_set(Interpreter *interp, const char *name, Value val)
{
    Frame *frame = interp->current_frame;
    int idx = var_hash(name, frame->var_size);

    RuntimeVar *v = frame->vars[idx];
    while (v)
    {
        if (strcmp(v->name, name) == 0)
        {
            v->value = val;
            return;
        }
        v = v->next;
    }

    RuntimeVar *nv = (RuntimeVar *)calloc(1, sizeof(RuntimeVar));
    nv->name = strdup(name);
    nv->value = val;
    nv->next = frame->vars[idx];
    frame->vars[idx] = nv;
}

static Value var_get(Interpreter *interp, const char *name)
{
    Frame *frame = interp->current_frame;

    while (frame)
    {
        int idx = var_hash(name, frame->var_size);
        RuntimeVar *v = frame->vars[idx];

        while (v)
        {
            if (strcmp(v->name, name) == 0)
            {
                return v->value;
            }
            v = v->next;
        }

        frame = frame->prev;
    }

    Value val;
    val.kind = VAL_NOTHING;
    return val;
}

/* Resolve an IROperand to a runtime value */
static Value resolve(Interpreter *interp, IROperand op)
{
    Value v;
    switch (op.kind)
    {
    case OPERAND_INT:
    {
        v.kind = VAL_INT;
        v.as.int_val = op.val.int_val;
        return v;
    }
    case OPERAND_FLOAT:
    {
        v.kind = VAL_FLOAT;
        v.as.float_val = op.val.float_val;
        return v;
    }
    case OPERAND_STRING:
    {
        v.kind = VAL_STRING;
        v.as.str_val = op.val.str_val;
        return v;
    }
    case OPERAND_TEMP:
    {
        return interp->temps[op.val.temp_id];
    }
    case OPERAND_VAR:
    {
        return var_get(interp, op.val.name);
    }
    case OPERAND_LABEL:
    {
        v.kind = VAL_NOTHING;
        return v;
    }
    default:
    {
        v.kind = VAL_NOTHING;
        return v;
    }
    }
}

/* Print a runtime value */
static void print_value(Value v)
{
    switch (v.kind)
    {
    case VAL_INT:
        printf("%d", v.as.int_val);
        break;
    case VAL_FLOAT:
        printf("%g", v.as.float_val);
        break;
    case VAL_STRING:
        printf("%s", v.as.str_val);
        break;
    case VAL_BOOL:
        printf("%s", v.as.bool_val ? "true" : "false");
        break;
    case VAL_NOTHING:
        printf("nothing");
        break;
    }
}

/* Store value into a dest operand */
static void store_dest(Interpreter *interp, IROperand dest, Value val)
{
    switch (dest.kind)
    {
    case OPERAND_TEMP:
    {
        interp->temps[dest.val.temp_id] = val;
        break;
    }
    case OPERAND_VAR:
    {
        var_set(interp, dest.val.name, val);
        break;
    }
    default:
        break;
    }
}

/* Arithematic helpers */
static Value val_add(Value a, Value b)
{
    Value r;
    if (a.kind == VAL_FLOAT || b.kind == VAL_FLOAT)
    {
        r.kind = VAL_FLOAT;
        double av = a.kind == VAL_FLOAT ? a.as.float_val : a.as.int_val;
        double bv = b.kind == VAL_FLOAT ? b.as.float_val : b.as.int_val;
        r.as.float_val = av + bv;
    }
    else
    {
        r.kind = VAL_INT;
        r.as.int_val = a.as.int_val + b.as.int_val;
    }
    return r;
}

static Value val_sub(Value a, Value b)
{
    Value r;
    if (a.kind == VAL_FLOAT || b.kind == VAL_FLOAT)
    {
        r.kind = VAL_FLOAT;
        double av = a.kind == VAL_FLOAT ? a.as.float_val : a.as.int_val;
        double bv = b.kind == VAL_FLOAT ? b.as.float_val : b.as.int_val;
        r.as.float_val = av - bv;
    }
    else
    {
        r.kind = VAL_INT;
        r.as.int_val = a.as.int_val - b.as.int_val;
    }
    return r;
}

static Value val_mul(Value a, Value b)
{
    Value r;
    if (a.kind == VAL_FLOAT || b.kind == VAL_FLOAT)
    {
        r.kind = VAL_FLOAT;
        double av = a.kind == VAL_FLOAT ? a.as.float_val : a.as.int_val;
        double bv = b.kind == VAL_FLOAT ? b.as.float_val : b.as.int_val;
        r.as.float_val = av * bv;
    }
    else
    {
        r.kind = VAL_INT;
        r.as.int_val = a.as.int_val * b.as.int_val;
    }
    return r;
}

static Value val_div(Value a, Value b)
{
    Value r;
    double av = a.kind == VAL_FLOAT ? a.as.float_val : a.as.int_val;
    double bv = b.kind == VAL_FLOAT ? b.as.float_val : b.as.int_val;
    if (bv == 0)
    {
        fprintf(stderr, "Runtime error : division by zero\n");
        r.kind = VAL_INT;
        r.as.int_val = 0;
        return r;
    }
    if (a.kind == VAL_FLOAT || b.kind == VAL_FLOAT)
    {
        r.kind = VAL_FLOAT;
        r.as.float_val = av / bv;
    }
    else
    {
        r.kind = VAL_INT;
        r.as.int_val = (int)(av / bv);
    }
    return r;
}

static Value val_compare(Value a, Value b, OpCode op)
{
    Value r;
    r.kind = VAL_BOOL;
    double av = a.kind == VAL_FLOAT ? a.as.float_val : a.as.int_val;
    double bv = b.kind == VAL_FLOAT ? b.as.float_val : b.as.int_val;
    switch (op)
    {
    case IR_EQ:
        r.as.bool_val = (av == bv);
        break;
    case IR_NEQ:
        r.as.bool_val = (av != bv);
        break;
    case IR_LT:
        r.as.bool_val = (av < bv);
        break;
    case IR_GT:
        r.as.bool_val = (av > bv);
        break;
    case IR_LTE:
        r.as.bool_val = (av <= bv);
        break;
    case IR_GTE:
        r.as.bool_val = (av >= bv);
        break;
    default:
        r.as.bool_val = 0;
    }
    return r;
}

/* Find label position in IR */
static int find_label(IRProgram *program, const char *name)
{
    for (int i = 0; i < program->count; i++)
    {
        IRInstruction *in = &program->instructions[i];
        if (in->op == IR_LABEL && in->dest.kind == OPERAND_LABEL && strcmp(in->dest.val.name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* Execute a range of IR instructions */
static void execute(Interpreter *interp, IRProgram *program, int start, int end)
{
    int i = start;
    while (i < end && !interp->returning)
    {
        IRInstruction *in = &program->instructions[i];

        switch (in->op)
        {
        case IR_STORE:
        {
            Value val = resolve(interp, in->src1);
            store_dest(interp, in->dest, val);
            break;
        }

        case IR_LOAD:
        {
            /* skip param-load instructions — handled by IR_CALL */
            if (in->src1.kind != OPERAND_INT)
            {
                Value val = resolve(interp, in->src1);
                store_dest(interp, in->dest, val);
            }
            break;
        }
        case IR_ADD:
        {
            Value a = resolve(interp, in->src1);
            Value b = resolve(interp, in->src2);
            store_dest(interp, in->dest, val_add(a, b));
            break;
        }

        case IR_SUB:
        {
            Value a = resolve(interp, in->src1);
            Value b = resolve(interp, in->src2);
            store_dest(interp, in->dest, val_sub(a, b));
            break;
        }

        case IR_MUL:
        {
            Value a = resolve(interp, in->src1);
            Value b = resolve(interp, in->src2);
            store_dest(interp, in->dest, val_mul(a, b));
            break;
        }

        case IR_DIV:
        {
            Value a = resolve(interp, in->src1);
            Value b = resolve(interp, in->src2);
            store_dest(interp, in->dest, val_div(a, b));
            break;
        }

        case IR_EQ:
        case IR_NEQ:
        case IR_LT:
        case IR_GT:
        case IR_LTE:
        case IR_GTE:
        {
            Value a = resolve(interp, in->src1);
            Value b = resolve(interp, in->src2);
            store_dest(interp, in->dest, val_compare(a, b, in->op));
            break;
        }

        case IR_NEG:
        {
            Value a = resolve(interp, in->src1);
            Value r;
            if (a.kind == VAL_FLOAT)
            {
                r.kind = VAL_FLOAT;
                r.as.float_val = -a.as.float_val;
            }
            else
            {
                r.kind = VAL_INT;
                r.as.int_val = -a.as.int_val;
            }
            store_dest(interp, in->dest, r);
            break;
        }

        case IR_LABEL:
            /* labels are no-ops at runtime */
            break;

        case IR_JUMP:
        {
            const char *label = in->dest.val.name;
            int target = find_label(program, label);
            if (target >= 0)
            {
                i = target;
                continue;
            }
            break;
        }

        case IR_JUMPF:
        {
            Value cond = resolve(interp, in->src1);
            int is_false = 0;
            if (cond.kind == VAL_BOOL)
            {
                is_false = !cond.as.bool_val;
            }
            else if (cond.kind == VAL_INT)
            {
                is_false = !cond.as.int_val;
            }

            if (is_false)
            {
                const char *label = in->dest.val.name;
                int target = find_label(program, label);
                if (target >= 0)
                {
                    i = target;
                    continue;
                }
            }
            break;
        }

        case IR_PARAM:
        {
            Value val = resolve(interp, in->dest);
            interp->call_stack[interp->call_top++] = val;
            break;
        }

        case IR_CALL:
        {
            const char *func_name = in->src1.val.name;
            int arg_count = in->src2.val.int_val;

            int func_start = find_label(program, func_name);
            if (func_start < 0)
            {
                fprintf(stderr,
                        "Runtime error: undefined function '%s'\n",
                        func_name);
                break;
            }

            int func_end = program->count;
            for (int j = func_start + 1; j < program->count; j++)
            {
                if (program->instructions[j].op == IR_LABEL &&
                    program->instructions[j].dest.kind == OPERAND_LABEL)
                {
                    const char *lname = program->instructions[j].dest.val.name;
                    if (strcmp(lname, "__main") == 0 || lname[0] != 'L')
                    {
                        func_end = j;
                        break;
                    }
                }
            }

            /* collect args from call stack */
            Value args[32];
            int arg_base = interp->call_top - arg_count;
            for (int j = 0; j < arg_count; j++)
                args[j] = interp->call_stack[arg_base + j];

            /* save caller state */
            int saved_top = arg_base;
            int saved_returning = interp->returning;
            Value saved_retval = interp->return_val;

            /* save caller frame */
            Frame *saved_frame = interp->current_frame;

            /* save caller temps */
            Value *saved_temps = interp->temps;
            int saved_temp_size = interp->temp_size;

            /* create new local frame for this function call */
            interp->current_frame = frame_create(saved_frame);

            /* create fresh temp storage for callee */
            interp->temps = (Value *)calloc(saved_temp_size, sizeof(Value));
            interp->temp_size = saved_temp_size;

            /* load params into local variables */
            int param_idx = 0;
            for (int j = func_start + 1; j < func_end; j++)
            {
                IRInstruction *pi = &program->instructions[j];
                if (pi->op == IR_LOAD &&
                    pi->dest.kind == OPERAND_VAR &&
                    pi->src1.kind == OPERAND_INT)
                {
                    if (param_idx < arg_count)
                        var_set(interp, pi->dest.val.name,
                                args[pi->src1.val.int_val]);
                    param_idx++;
                }
                else
                {
                    break;
                }
            }

            interp->returning = 0;
            interp->return_val.kind = VAL_NOTHING;
            interp->call_top = saved_top;

            execute(interp, program, func_start + 1, func_end);

            Value result = interp->return_val;

            /* destroy function frame and restore caller frame */
            Frame *dead = interp->current_frame;
            interp->current_frame = saved_frame;
            frame_destroy(dead);

            /* destroy callee temps and restore caller temps */
            free(interp->temps);
            interp->temps = saved_temps;
            interp->temp_size = saved_temp_size;

            interp->returning = saved_returning;
            interp->return_val = saved_retval;

            store_dest(interp, in->dest, result);
            break;
        }

        case IR_RETURN:
        {
            Value val = resolve(interp, in->dest);
            interp->return_val = val;
            interp->returning = 1;
            return;
        }

        case IR_SHOW:
        {
            Value val = resolve(interp, in->dest);
            print_value(val);
            printf("\n");
            break;
        }

        default:
            break;
        }
        i++;
    }
}

/*---------------------------------
MAIN RUN FUNCTION
----------------------------------*/
void interp_run(Interpreter *interp, IRProgram *program)
{
    /* find __main label and execute from there */
    int main_start = find_label(program, "__main");
    if (main_start < 0)
    {
        fprintf(stderr, "Runtime error: no entry point found\n");
        return;
    }

    /* execute from __main to end */
    execute(interp, program, main_start + 1, program->count);
}