#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "../include/tokens.h"
#include "../include/ast.h"
#include "../include/ir.h"
#include "../include/interpreter.h"


/*forward declerations*/
typedef struct{
    const char* source;
    int pos, line, column, length;
}Scanner;

void            scanner_init(Scanner* s, const char* source);
Token           next_token(Scanner* s);
ASTNode*        parse(Token* tokens, int count);
int             analyze(ASTNode* root);
IRProgram*      generate_ir(ASTNode* root);


/* read entire file into a string */
char* read_file(const char* path){
    FILE* f = fopen(path,"r");
    if(!f){
        fprintf(stderr,"ERROR: cannot open file '%s'\n",path);
        exit(1);
    }
    fseek(f,0,SEEK_END);
    long size = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(size+1);
    fread(buffer,1,size,f);
    buffer[size] = '\0';
    fclose(f);

    /* strip BOM */
    char* start = buffer;
    if ((unsigned char)start[0] == 0xEF &&
        (unsigned char)start[1] == 0xBB &&
        (unsigned char)start[2] == 0xBF)
        start += 3;
    return start;
}

/* main */
int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr,"Usage: lang <file.learn>\n");
        return 1;
    }

    /* Stage 1 - SCAN */
    char* source = read_file(argv[1]);
    Scanner s;
    scanner_init(&s,source);
    
    Token* tokens = (Token*)malloc(sizeof(Token) * 4096);
    int count = 0;
    Token t;
    do{
        t = next_token(&s);
        tokens[count++] = t;
    }while(t.type != TOKEN_EOF);

    /* Stage 2 : Parse */
    ASTNode* ast = parse(tokens,count);

    /* Stage 3 : Semantic Analysis */
    int errors = analyze(ast);
    if(errors){
        fprintf(stderr,"Compilation failed./n");
        return 1;
    }
    
    /* Stage 4 : IR Generation */
    IRProgram* ir = generate_ir(ast);
    
    /* Stage 5 : Execute */
    Interpreter* interp = interp_create();
    interp_run(interp,ir);
    interp_destroy(interp);

    ir_destroy(ir);
    free_ast(ast);
    free(tokens);
    return 0;
}
