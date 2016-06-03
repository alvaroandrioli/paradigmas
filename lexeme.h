#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

typedef struct tVar {
    char *nome;
    int tipo;
}var;

char *obterNome(char *, FILE *);
int lookup(char);
int isReservedWord();
void addLastFuncion(var);
void remLastFuncion();
var *varExiste(char *);
int variavel(FILE *);
int tipoVar();
int declaraVar(FILE *);
void programa(FILE *);
void blocoDePrograma(FILE *);
void instrucoes(FILE *);
void comando(FILE *);
void calc(FILE *);
int expr(FILE *);
int term(FILE *);
int factor(FILE *);
int exprLogica(FILE *);
void condicional(FILE *);
void loop(FILE *);
int checkSintaxe(int, FILE *);
void resultadoSintaxe();
void addChar();
void getChar(FILE *);
void getNonBlank(FILE *);
int lex(int, FILE *);

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

#define INT_LIT 10
#define IDENT 11
#define REAL_LIT 12

#define DOT 17
#define MOD_OP 18
#define LOG_OP 19
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define SEMICOLON 27
#define COMMA 28
#define TWO_POINTS 29

#define PROGRAMA_CODE 31
#define INICIO_CODE 32
#define FIM_CODE 33
#define VAR_CODE 34
#define INT_CODE 35
#define REAL_CODE 36
#define SE_CODE 37
#define ENTAO_CODE 38
#define SENAO_CODE 39
#define CASO_CODE 40
#define ESCOLHA_CODE 41
#define ENQUANTO_CODE 42
#define FACA_CODE 43
#define REPITA_CODE 44
#define ATE_CODE 45
#define PARA_CODE 46
#define LOGICA_CODE 47
#define OP_CODE 48
