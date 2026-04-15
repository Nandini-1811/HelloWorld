#ifndef IR_H
#define IR_H

#include<stdlib.h>

/*----------------------------------------
Stage 4: Intermediate Code Generation
------------------------------------------*/

typedef enum{
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_EQ, 
    IR_NEQ, 
    IR_LT, 
    IR_GT, 
    IR_LTE, 
    IR_GTE,
    IR_STORE,
    IR_LOAD,
    IR_JUMP, 
    IR_JUMPF,
    IR_LABEL,
    IR_CALL, 
    IR_PARAM,
    IR_RETURN,
    IR_SHOW,
    IR_NEG,
}OpCode;

typedef enum {
    OPERAND_TEMP,    /* t0, t1, t2 — compiler temporaries */
    OPERAND_VAR,     /* x, name, result — named variables  */
    OPERAND_INT,     /* 42                                  */
    OPERAND_FLOAT,   /* 3.14                                */
    OPERAND_STRING,  /* "hello"                             */
    OPERAND_LABEL,   /* L0, L1 — jump targets               */
    OPERAND_NONE,    /* empty — not all instructions use 3  */
} OperandKind;

typedef struct {
    OperandKind kind;
    union {
        int    temp_id;   /* for OPERAND_TEMP  */
        char*  name;      /* for OPERAND_VAR and OPERAND_LABEL */
        int    int_val;   /* for OPERAND_INT   */
        double float_val; /* for OPERAND_FLOAT */
        char*  str_val;   /* for OPERAND_STRING */
    } val;
} IROperand;

typedef struct {
    OpCode   op;
    IROperand dest;    /* where result goes     */
    IROperand src1;    /* first operand         */
    IROperand src2;    /* second operand        */
} IRInstruction;

typedef struct {
    IRInstruction* instructions;
    int            count;
    int            capacity;
    int            temp_count;   /* next temp ID to use  */
    int            label_count;  /* next label ID to use */
} IRProgram;


IRProgram* ir_create(void);
void       ir_emit(IRProgram* p, IRInstruction instr);
int        ir_new_temp(IRProgram* p);
char*      ir_new_label(IRProgram* p);
void       ir_print(IRProgram* p);
void       ir_destroy(IRProgram* p);

/* operand constructors */
IROperand  op_temp(int id);
IROperand  op_var(char* name);
IROperand  op_int(int val);
IROperand  op_float(double val);
IROperand  op_str(char* val);
IROperand  op_label(char* name);
IROperand  op_none(void);

#endif /* IR_H */