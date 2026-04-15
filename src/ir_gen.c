#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ir.h"
#include "../include/ast.h"

/*─────────────────────────────────────────
  HelloWorld IR Generator
  Stage 4 — Intermediate Code Generation

  Walks the validated AST and emits
  3-address IR instructions.
─────────────────────────────────────────*/


/* ── IR Program lifecycle ── */

IRProgram* ir_create(void) {
    IRProgram* p = (IRProgram*)calloc(1, sizeof(IRProgram));
    p->capacity     = 256;
    p->instructions = (IRInstruction*)malloc(
                        sizeof(IRInstruction) * p->capacity);
    p->count        = 0;
    p->temp_count   = 0;
    p->label_count  = 0;
    return p;
}

void ir_emit(IRProgram* p, IRInstruction instr) {
    /* grow if needed */
    if (p->count >= p->capacity) {
        p->capacity *= 2;
        p->instructions = (IRInstruction*)realloc(
            p->instructions,
            sizeof(IRInstruction) * p->capacity);
    }
    p->instructions[p->count++] = instr;
}

/* allocate next temporary — returns id like 0, 1, 2... */
int ir_new_temp(IRProgram* p) {
    return p->temp_count++;
}

/* allocate next label — returns string like "L0", "L1"... */
char* ir_new_label(IRProgram* p) {
    char buf[32];
    snprintf(buf, sizeof(buf), "L%d", p->label_count++);
    return strdup(buf);
}

void ir_destroy(IRProgram* p) {
    free(p->instructions);
    free(p);
}


/* ── Operand constructors ── */

IROperand op_temp(int id) {
    IROperand o;
    o.kind        = OPERAND_TEMP;
    o.val.temp_id = id;
    return o;
}

IROperand op_var(char* name) {
    IROperand o;
    o.kind     = OPERAND_VAR;
    o.val.name = name;
    return o;
}

IROperand op_int(int val) {
    IROperand o;
    o.kind        = OPERAND_INT;
    o.val.int_val = val;
    return o;
}

IROperand op_float(double val) {
    IROperand o;
    o.kind          = OPERAND_FLOAT;
    o.val.float_val = val;
    return o;
}

IROperand op_str(char* val) {
    IROperand o;
    o.kind        = OPERAND_STRING;
    o.val.str_val = val;
    return o;
}

IROperand op_label(char* name) {
    IROperand o;
    o.kind     = OPERAND_LABEL;
    o.val.name = name;
    return o;
}

IROperand op_none(void) {
    IROperand o;
    o.kind = OPERAND_NONE;
    return o;
}


/* ── Helper to print one operand ── */
static void print_operand(IROperand o) {
    switch (o.kind) {
        case OPERAND_TEMP:   printf("t%d",    o.val.temp_id);  break;
        case OPERAND_VAR:    printf("%s",     o.val.name);     break;
        case OPERAND_INT:    printf("%d",     o.val.int_val);  break;
        case OPERAND_FLOAT:  printf("%g",     o.val.float_val);break;
        case OPERAND_STRING: printf("\"%s\"", o.val.str_val);  break;
        case OPERAND_LABEL:  printf("%s",     o.val.name);     break;
        case OPERAND_NONE:   break;
    }
}


/* ── IR Printer ── */
void ir_print(IRProgram* p) {
    printf("\n=== IR Instructions ===\n");
    for (int i = 0; i < p->count; i++) {
        IRInstruction* in = &p->instructions[i];

        /* labels print differently — no indentation */
        if (in->op == IR_LABEL) {
            print_operand(in->dest);
            printf(":\n");
            continue;
        }

        printf("  ");

        switch (in->op) {
            case IR_ADD:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" + ");  print_operand(in->src2); break;
            case IR_SUB:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" - ");  print_operand(in->src2); break;
            case IR_MUL:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" * ");  print_operand(in->src2); break;
            case IR_DIV:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" / ");  print_operand(in->src2); break;
            case IR_EQ:     print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" == "); print_operand(in->src2); break;
            case IR_NEQ:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" != "); print_operand(in->src2); break;
            case IR_LT:     print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" < ");  print_operand(in->src2); break;
            case IR_GT:     print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" > ");  print_operand(in->src2); break;
            case IR_LTE:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" <= "); print_operand(in->src2); break;
            case IR_GTE:    print_operand(in->dest); printf(" = "); print_operand(in->src1); printf(" >= "); print_operand(in->src2); break;
            case IR_NEG:    print_operand(in->dest); printf(" = -"); print_operand(in->src1); break;
            case IR_STORE:  print_operand(in->dest); printf(" = "); print_operand(in->src1); break;
            case IR_LOAD:   print_operand(in->dest); printf(" = load "); print_operand(in->src1); break;
            case IR_JUMP:   printf("jump "); print_operand(in->dest); break;
            case IR_JUMPF:  printf("jumpf "); print_operand(in->src1); printf(" -> "); print_operand(in->dest); break;
            case IR_PARAM:  printf("param "); print_operand(in->dest); break;
            case IR_CALL:   print_operand(in->dest); printf(" = call "); print_operand(in->src1); printf(" ("); print_operand(in->src2); printf(" args)"); break;
            case IR_RETURN: printf("return "); print_operand(in->dest); break;
            case IR_SHOW:   printf("show "); print_operand(in->dest); break;
            default: break;
        }
        printf("\n");
    }
}

/*─────────────────────────────────────────
  AST → IR TRANSLATION
─────────────────────────────────────────*/

/* forward declaration */
static IROperand gen_expr(IRProgram* p, ASTNode* node);
static void      gen_stmt(IRProgram* p, ASTNode* node);


/* ── Expression → IR ── */
/* returns the operand where the result lives */

static IROperand gen_expr(IRProgram* p, ASTNode* node) {
    if (!node) return op_none();

    switch (node->type) {

        /* literals — just return the operand directly */
        case NODE_NUMBER_LIT:
            return op_int(node->as.number.value);

        case NODE_DECIMAL_LIT:
            return op_float(node->as.decimal.value);

        case NODE_STRING_LIT:
            return op_str(node->as.string.value);

        case NODE_BOOL_LIT:
            return op_int(node->as.boolean.value);

        case NODE_IDENTIFIER:
            return op_var(node->as.identifier.name);

        case NODE_UNARY_EXPR: {
            IROperand operand = gen_expr(p, node->as.unary.operand);
            int t = ir_new_temp(p);
            ir_emit(p, (IRInstruction){
                IR_NEG, op_temp(t), operand, op_none()
            });
            return op_temp(t);
        }

        case NODE_BINARY_EXPR: {
            IROperand left  = gen_expr(p, node->as.binary.left);
            IROperand right = gen_expr(p, node->as.binary.right);
            char* op = node->as.binary.op;
            int t = ir_new_temp(p);

            OpCode code;
            if      (strcmp(op, "+")  == 0) code = IR_ADD;
            else if (strcmp(op, "-")  == 0) code = IR_SUB;
            else if (strcmp(op, "*")  == 0) code = IR_MUL;
            else if (strcmp(op, "/")  == 0) code = IR_DIV;
            else if (strcmp(op, "==") == 0) code = IR_EQ;
            else if (strcmp(op, "!=") == 0) code = IR_NEQ;
            else if (strcmp(op, "<")  == 0) code = IR_LT;
            else if (strcmp(op, ">")  == 0) code = IR_GT;
            else if (strcmp(op, "<=") == 0) code = IR_LTE;
            else if (strcmp(op, ">=") == 0) code = IR_GTE;
            else                            code = IR_ADD;

            ir_emit(p, (IRInstruction){
                code, op_temp(t), left, right
            });
            return op_temp(t);
        }

        case NODE_CALL_EXPR: {
            /* emit PARAM for each argument first */
            for (int i = 0; i < node->as.call.arg_count; i++) {
                IROperand arg = gen_expr(p, node->as.call.args[i]);
                ir_emit(p, (IRInstruction){
                    IR_PARAM, arg, op_none(), op_none()
                });
            }
            /* emit CALL instruction */
            int t = ir_new_temp(p);
            ir_emit(p, (IRInstruction){
                IR_CALL,
                op_temp(t),
                op_var(node->as.call.name),
                op_int(node->as.call.arg_count)
            });
            return op_temp(t);
        }

        default:
            return op_none();
    }
}


/* ── Statement → IR ── */

static void gen_stmt(IRProgram* p, ASTNode* node) {
    if (!node) return;

    switch (node->type) {

        case NODE_VAR_DECL: {
            IROperand val = gen_expr(p, node->as.var_decl.value);
            ir_emit(p, (IRInstruction){
                IR_STORE,
                op_var(node->as.var_decl.name),
                val,
                op_none()
            });
            break;
        }

        case NODE_ASSIGN: {
            IROperand val = gen_expr(p, node->as.assign.value);
            ir_emit(p, (IRInstruction){
                IR_STORE,
                op_var(node->as.assign.name),
                val,
                op_none()
            });
            break;
        }

        case NODE_FUNC_DECL: {
            /* emit a label for the function entry point */
            ir_emit(p, (IRInstruction){
                IR_LABEL,
                op_label(node->as.func_decl.name),
                op_none(), op_none()
            });
            /* generate body */
            gen_stmt(p, node->as.func_decl.body);
            break;
        }

        case NODE_RETURN: {
            IROperand val = gen_expr(p, node->as.ret.value);
            ir_emit(p, (IRInstruction){
                IR_RETURN, val, op_none(), op_none()
            });
            break;
        }

        case NODE_IF: {
            /* generate condition */
            IROperand cond = gen_expr(p, node->as.if_stmt.condition);

            char* else_label = ir_new_label(p);
            char* end_label  = ir_new_label(p);

            /* if condition false jump to else */
            ir_emit(p, (IRInstruction){
                IR_JUMPF,
                op_label(else_label),
                cond,
                op_none()
            });

            /* then block */
            gen_stmt(p, node->as.if_stmt.then_block);

            /* jump over else block */
            ir_emit(p, (IRInstruction){
                IR_JUMP,
                op_label(end_label),
                op_none(), op_none()
            });

            /* else label */
            ir_emit(p, (IRInstruction){
                IR_LABEL,
                op_label(else_label),
                op_none(), op_none()
            });

            /* else block if exists */
            if (node->as.if_stmt.else_block)
                gen_stmt(p, node->as.if_stmt.else_block);

            /* end label */
            ir_emit(p, (IRInstruction){
                IR_LABEL,
                op_label(end_label),
                op_none(), op_none()
            });
            break;
        }

        case NODE_REPEAT: {
            char* start_label = ir_new_label(p);
            char* end_label   = ir_new_label(p);

            /* init — let i is num = 0 */
            gen_stmt(p, node->as.repeat.init);

            /* loop start label */
            ir_emit(p, (IRInstruction){
                IR_LABEL,
                op_label(start_label),
                op_none(), op_none()
            });

            /* condition check */
            IROperand cond = gen_expr(p, node->as.repeat.condition);
            ir_emit(p, (IRInstruction){
                IR_JUMPF,
                op_label(end_label),
                cond,
                op_none()
            });

            /* body */
            gen_stmt(p, node->as.repeat.body);

            /* step — i = i + 1 */
            gen_stmt(p, node->as.repeat.step);

            /* jump back to start */
            ir_emit(p, (IRInstruction){
                IR_JUMP,
                op_label(start_label),
                op_none(), op_none()
            });

            /* end label */
            ir_emit(p, (IRInstruction){
                IR_LABEL,
                op_label(end_label),
                op_none(), op_none()
            });
            break;
        }

        case NODE_SHOW: {
            IROperand val = gen_expr(p, node->as.show.value);
            ir_emit(p, (IRInstruction){
                IR_SHOW, val, op_none(), op_none()
            });
            break;
        }

        case NODE_BLOCK: {
            for (int i = 0; i < node->as.block.count; i++)
                gen_stmt(p, node->as.block.statements[i]);
            break;
        }

        case NODE_CALL_EXPR: {
            gen_expr(p, node);
            break;
        }

        default:
            break;
    }
}


/*─────────────────────────────────────────
  MAIN ENTRY POINT
─────────────────────────────────────────*/

IRProgram* generate_ir(ASTNode* root) {
    IRProgram* p = ir_create();

    for (int i = 0; i < root->as.program.count; i++)
        gen_stmt(p, root->as.program.statements[i]);

    return p;
}