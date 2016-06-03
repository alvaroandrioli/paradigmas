#include "lexeme.h"

int charClass, lexLen, token, nextToken, tokenOK, nVars, varType;
int quantInicios, quantParenteses, quantFuncions;
unsigned int quantOPAfterClosed;
char *lexeme, nextChar, *lastIdent, *nomePrograma;
var *declaredVars, *pilhaFuncions, lastFunction, lastClosed;

char *obterNome(char *nomeExt, FILE *arq) {
    char *nome = NULL, *aux, c[2];
    int i = 0;

    if(nomeExt != NULL) delete[] nomeExt;

    c[1] = '\0';

    do {
        c[0] = getc(arq);

        if(c[0] != '\n') {
            aux = nome;
            nome = new char [i+2];
            strcpy(nome, "");

            if(aux != NULL) {
                strcpy(nome, aux);
                delete[] aux;
            }

            strcat(nome, c);

            i++;
        }
    } while(c[0] != '\n' || i == 0);

    return nome;
}

int lookup(char ch) {
    int tokenTemp = EOF;

    switch(ch) {
        case '.':
            tokenTemp = DOT;
        break;

        case '(':
            tokenTemp = LEFT_PAREN;
        break;

        case ')':
            tokenTemp = RIGHT_PAREN;
        break;

        case '+':
            tokenTemp = ADD_OP;
        break;

        case '-':
            tokenTemp = SUB_OP;
        break;

        case '*':
            tokenTemp = MULT_OP;
        break;

        case '/':
            tokenTemp = DIV_OP;
        break;

        case '=':
            tokenTemp = ASSIGN_OP;
        break;

        case ';':
            tokenTemp = SEMICOLON;
        break;

        case ',':
            tokenTemp = COMMA;
        break;

        case ':':
            tokenTemp = TWO_POINTS;
        break;

        case '>':
        case '<':
            tokenTemp = LOG_OP;
        break;

        default:
            tokenTemp = EOF;
    }

    return tokenTemp;
}

int isReservedWord() {
    int tokenTemp = IDENT;

    if(!strcmp(lexeme, "resto"))
        tokenTemp = MOD_OP;

    else if(!strcmp(lexeme, "programa"))
        tokenTemp = PROGRAMA_CODE;

    else if(!strcmp(lexeme, "inicio") || !strcmp(lexeme, "início"))
        tokenTemp = INICIO_CODE;

    else if(!strcmp(lexeme, "fim"))
        tokenTemp = FIM_CODE;

    else if(!strcmp(lexeme, "var"))
        tokenTemp = VAR_CODE;

    else if(!strcmp(lexeme, "inteiro"))
        tokenTemp = INT_CODE;

    else if(!strcmp(lexeme, "real"))
        tokenTemp = REAL_CODE;

    else if(!strcmp(lexeme, "se"))
        tokenTemp = SE_CODE;

    else if(!strcmp(lexeme, "entao") || !strcmp(lexeme, "então"))
        tokenTemp = ENTAO_CODE;

    else if(!strcmp(lexeme, "senao") || !strcmp(lexeme, "senão"))
        tokenTemp = SENAO_CODE;

    else if(!strcmp(lexeme, "caso"))
        tokenTemp = CASO_CODE;

    else if(!strcmp(lexeme, "escolha"))
        tokenTemp = ESCOLHA_CODE;

    else if(!strcmp(lexeme, "enquanto"))
        tokenTemp = ENQUANTO_CODE;

    else if(!strcmp(lexeme, "faca") || !strcmp(lexeme, "faça"))
        tokenTemp = FACA_CODE;

    else if(!strcmp(lexeme, "repita"))
        tokenTemp = REPITA_CODE;

    else if(!strcmp(lexeme, "ate") || !strcmp(lexeme, "até"))
        tokenTemp = ATE_CODE;

    else if(!strcmp(lexeme, "para"))
        tokenTemp = PARA_CODE;

    return tokenTemp;
}

void addLastFuncion(var funcion) {
    int i = quantFuncions;
    var *tempPilha = pilhaFuncions;

    pilhaFuncions = new var [++quantFuncions];

    if(tempPilha != NULL) {
        for(i = 0; i < (quantFuncions-1); i++) {
            pilhaFuncions[i] = tempPilha[i];
        }

        delete[] tempPilha;
    }

    pilhaFuncions[i] = funcion;

    printf("Iniciando \"%s\"\n", pilhaFuncions[i].nome);
}

void remLastFuncion() {
    int i = quantFuncions;
    var *tempPilha = pilhaFuncions;

    pilhaFuncions = new var [--quantFuncions];

    if(tempPilha != NULL) {
        for(i = 0; i < quantFuncions; i++) {
            pilhaFuncions[i] = tempPilha[i];
        }

        if(lastClosed.nome != NULL && lastClosed.nome != lastFunction.nome)
            delete[] lastClosed.nome;

        lastClosed.nome = tempPilha[i].nome;
        lastClosed.tipo = tempPilha[i].tipo;
        delete[] tempPilha;
    }

    printf("Finalizando \"%s\"\n", lastClosed.nome);
    quantOPAfterClosed = 0;
}

var *varExiste(char *nome) {
    for(int i = 0; i < nVars; i++)
        if(!strcmp(declaredVars[i].nome, nome)) return &declaredVars[i];

    return NULL;
}

int variavel(FILE *arq) {
    token = nextToken;

    if(lex(token, arq) == IDENT) {
        if(varExiste(lexeme) != NULL) return 1;

        return 0;
    }

    return -1;
}

int tipoVar() {
    switch(nextToken) {
        case INT_CODE:
        case REAL_CODE:
            return nextToken;
    }

    return -1;
}

int declaraVar(FILE *arq) {
    var *tempDeclaredVars;
    int aux;

    do {
        aux = variavel(arq);

        if(nextToken == TWO_POINTS) {
            token = nextToken;
            lex(token, arq);

            aux = 1;
        }

        switch(aux) {
            case 0:
                tempDeclaredVars = declaredVars;

                declaredVars = new var [nVars+1];

                declaredVars[nVars].nome = new char [strlen(lexeme)];

                strcpy(declaredVars[nVars].nome, lexeme);

                for(int i = 0; i < nVars; i++) {
                    declaredVars[i] = tempDeclaredVars[i];
                }

                nVars++;

                if(tempDeclaredVars != NULL) {
                    delete[] tempDeclaredVars;
                }
            break;

            case 1:
                aux = tipoVar();

                if(aux != -1) {
                    for(int i = 0; i < nVars; i++) {
                        declaredVars[i].tipo = aux;
                    }

                    token = nextToken;

                    if(lex(token, arq) != SEMICOLON) {
                        token = -SEMICOLON;
                        return -1;
                    }
                } else {
                    return 0;
                }
                break;

            default:
                if(nextToken != COMMA) {
                    return 0;
                }
            break;
        }
    } while(nextToken != SEMICOLON);

    return 1;
}

void programa(FILE *arq) {
    token = nextToken;
    nomePrograma = lexeme;

    if(lex(token, arq) == IDENT && quantFuncions == 0) {
        token = nextToken;

        if(lex(token, arq) != SEMICOLON) {
            token = -SEMICOLON;
            tokenOK = -1;
        } else {
            blocoDePrograma(arq);
        }
    } else {
        token = -PROGRAMA_CODE;
        tokenOK = -1;
    }
}

void blocoDePrograma(FILE *arq) {
    token = nextToken;

    addLastFuncion(lastFunction);

    while(lex(token, arq) == VAR_CODE) {
        tokenOK = declaraVar(arq);

        if(tokenOK != 1)
            break;
    }

    if(tokenOK == 1) {
        if(nextToken == INICIO_CODE) {
            instrucoes(arq);
        } else {
            comando(arq);
            remLastFuncion();
        }
    }
}

void instrucoes(FILE *arq) {
    do {
        token = nextToken;
        lex(token, arq);

        comando(arq);
    } while(nextToken != FIM_CODE && tokenOK == 1);
}

void comando(FILE *arq) {
    checkSintaxe(nextToken, arq);
}

void calc(FILE *arq) {
    if(varExiste(lexeme) != NULL) {
        token = nextToken;

        if(lex(token, arq) == TWO_POINTS) {
            token = nextToken;

            if(lex(token, arq) == ASSIGN_OP) {
                token = nextToken;
                lex(token, arq);

                if(!expr(arq)) {
                    tokenOK = 0;
                } else if(nextToken != SEMICOLON) {
                    token = -SEMICOLON;
                    tokenOK = -1;
                } else if(quantParenteses > 0) {
                    token = -RIGHT_PAREN;
                    tokenOK = -1;
                } else if(quantParenteses < 0) {
                    token = -LEFT_PAREN;
                    tokenOK = -1;
                }
            } else {
                token = -ASSIGN_OP;
                tokenOK = -1;
            }
        } else {
            token = -TWO_POINTS;
            tokenOK = -1;
        }
    } else {
        token = -VAR_CODE;
        tokenOK = -1;
    }
}

int expr(FILE *arq) {
    if(term(arq)) {
        if(nextToken == INT_LIT || nextToken == REAL_LIT || nextToken == IDENT ||
           nextToken == ADD_OP || nextToken == SUB_OP) {
            token = nextToken;

            while(lex(token, arq) == ADD_OP || nextToken == SUB_OP) {
                tokenOK = term(arq);

                if(tokenOK != 1)
                    return 0;

                token = nextToken;
            }
        }

        return 1;
    }

    return 0;
}

int term(FILE *arq) {
    if(factor(arq)) {
        if(nextToken == INT_LIT || nextToken == REAL_LIT || nextToken == IDENT ||
           nextToken == MULT_OP || nextToken == DIV_OP) {
            token = nextToken;

            while(lex(token, arq) == MULT_OP || nextToken == DIV_OP) {
                tokenOK = term(arq);

                if(tokenOK != 1)
                    return 0;

                token = nextToken;
            }
        }

        return 1;
    }

    return 0;
}

int factor(FILE *arq) {
    if(nextToken == INT_LIT || nextToken == REAL_LIT)
        return 1;
    else if(varExiste(lexeme) != NULL)
        return 1;
    else if(nextToken == LEFT_PAREN) {
        quantParenteses++;

        token = nextToken;
        lex(token, arq);

        while(expr(arq))
            if(tokenOK != 1)
                return 0;

        return 1;

    } else if(nextToken == RIGHT_PAREN) {
        quantParenteses--;

        token = nextToken;
        lex(token, arq);

        return 1;
    }

    return 0;
}

int exprLogica(FILE *arq) {
    if(expr(arq)) {
        if(nextToken != ENTAO_CODE && nextToken != FACA_CODE && nextToken != ATE_CODE) {
            if(nextToken == LOG_OP) {
                token = nextToken;

                if(lex(token, arq) == LOG_OP ||
                   nextToken == ASSIGN_OP) {
                        token = nextToken;
                        lex(token, arq);
                   }

            } else if(nextToken == ASSIGN_OP) {
                token = nextToken;
                lex(token, arq);
            } else {
                return 0;
            }

            if(!expr(arq)) return 0;
        }
        while(expr(arq))
            if(tokenOK != 1)
                return 0;

        return 1;
    }

    return 0;
}

void condicional(FILE *arq) {
    if(nextToken == SE_CODE) {
        token = nextToken;
        lex(token, arq);

        if(!exprLogica(arq)) {
            token = -LOGICA_CODE;
            tokenOK = -1;
        } else if(nextToken != ENTAO_CODE) {
            token = -ENTAO_CODE;
            tokenOK = -1;
        } else {
            blocoDePrograma(arq);
        }
    } else {
        token = -SE_CODE;
        tokenOK = -1;
    }
}

void loop(FILE *arq) {
    if(nextToken == ENQUANTO_CODE) {
        token = nextToken;
        lex(token, arq);

        if(!exprLogica(arq)) {
            token = -LOGICA_CODE;
            tokenOK = -1;
        } else if(nextToken != FACA_CODE) {
            token = -FACA_CODE;
            tokenOK = -1;
        } else {
            blocoDePrograma(arq);
        }
    } else {
        token = -ENQUANTO_CODE;
        tokenOK = -1;
    }
}

int checkSintaxe(int tokenExt, FILE *arq) {
    quantOPAfterClosed++;

    switch(tokenExt) {
        case PROGRAMA_CODE:
            programa(arq);
        break;

        case ENQUANTO_CODE:
            loop(arq);
        break;

        case SE_CODE:
            condicional(arq);
        break;

        case SENAO_CODE:
            if(lastClosed.tipo == SE_CODE && quantOPAfterClosed == 1)
                blocoDePrograma(arq);
            else
                tokenOK = 0;
        break;

        case IDENT:
            calc(arq);
        break;

        case FIM_CODE:
            token = nextToken;
            lex(token, arq);

            if(nextToken != SEMICOLON && quantFuncions > 1) {
                token = -SEMICOLON;
                tokenOK = -1;
            } else if(nextToken != DOT && quantFuncions == 1) {
                token = -DOT;
                tokenOK = -1;
            } else if(quantFuncions < 1) {
                token = -FIM_CODE;
                tokenOK = -1;
            }

            if(tokenOK == 1)
                remLastFuncion();

        default:
            quantOPAfterClosed--;
    }

    if(tokenOK != -1)
        token = (tokenOK == 1) ? tokenExt : (-token);

    return token;
}

/*int checkSintaxe(int tokenExt, FILE *arq) {
    int i;

    quantOPAfterClosed++;

    switch(tokenExt) {
        case PROGRAMA_CODE:
            if(nextToken == IDENT) {
                if(lex(tokenExt, arq) != SEMICOLON) {
                    token = -SEMICOLON;
                    tokenOK = -1;

                } else
                    tokenOK = 1;

            } else
                tokenOK = 0;

        break;

        case VAR_CODE:
            tempDeclaredVars = declaredVars;

            if(nextToken == IDENT) {
                declaredVars = new var [++nVars];

                declaredVars[nVars-1].nome = new char [strlen(lexeme)];

                strcpy(declaredVars[nVars-1].nome, lexeme);

                for(i = 0; i < (nVars-1); i++) {
                    declaredVars[i] = tempDeclaredVars[i];
                }

                if(tempDeclaredVars != NULL) {
                    delete[] tempDeclaredVars;
                    tempDeclaredVars = NULL;
                }

                token = nextToken;
                if(lex(tokenExt, arq) != TWO_POINTS) {
                    if(nextToken == COMMA) {
                        token = nextToken;
                        lex(tokenExt, arq);

                        if(checkSintaxe(tokenExt, arq) > EOF) {
                            declaredVars[i].tipo = varType;
                        }

                        tokenExt = COMMA;

                    } else {
                        token = -TWO_POINTS;
                        tokenOK = -1;
                    }
                } else {
                    token = nextToken;
                    if(lex(tokenExt, arq) == INT_CODE || nextToken == REAL_CODE) {
                        token = nextToken;

                        if(lex(tokenExt, arq) == SEMICOLON) {
                            varType = token;
                            declaredVars[i].tipo = varType;

                            tokenOK = 1;

                        } else {
                            token = -SEMICOLON;
                            tokenOK = -1;
                        }
                    } else
                        tokenOK = 0;

                }
            } else
                tokenOK = 0;

        break;

        case INICIO_CODE:
            quantInicios++;
            tokenOK = 1;

        break;

        case FIM_CODE:
            if(--quantInicios < 0) {
                token = -FIM_CODE;
                tokenOK = -1;
            } else {
                remLastFuncion();

                if(quantInicios == 0) {
                    if(nextToken == EOF) {
                        token = -DOT;
                        tokenOK = -1;
                    }
                } else {
                    if(nextToken != SEMICOLON) {
                        token = -SEMICOLON;
                        tokenOK = -1;
                    }
                }
            }
        break;

        case ENQUANTO_CODE:
            checkSintaxe(LOGICA_CODE, arq);

            if(tokenOK == 1) {
                if(quantParenteses > 0) {
                    token = -RIGHT_PAREN;
                    tokenOK = -1;
                } else if(quantParenteses < 0) {
                    token = -LEFT_PAREN;
                    tokenOK = -1;
                } else if(token != FACA_CODE) {
                    token = -token;
                    tokenOK = -1;
                } else {
                    token = lex(token, arq);
                    lex(token, arq);
                    if(token != INICIO_CODE) {
                        checkSintaxe(token, arq);
                        remLastFuncion();
                    } else {
                        checkSintaxe(token, arq);
                    }
                }
            }
        break;

        case SE_CODE:
            checkSintaxe(LOGICA_CODE, arq);

            if(tokenOK == 1) {
                if(quantParenteses > 0) {
                    token = -RIGHT_PAREN;
                    tokenOK = -1;
                } else if(quantParenteses < 0) {
                    token = -LEFT_PAREN;
                    tokenOK = -1;
                } else if(token != ENTAO_CODE) {
                    token = -token;
                    tokenOK = -1;
                } else {
                    token = lex(token, arq);
                    lex(token, arq);

                    if(token != INICIO_CODE) {
                        checkSintaxe(token, arq);
                        remLastFuncion();
                    } else {
                        checkSintaxe(token, arq);
                    }
                }
            }
        break;

        case SENAO_CODE:
            tokenOK = 1;

            if((lastClosed.tipo != SE_CODE && lastClosed.tipo != SENAO_CODE) || quantOPAfterClosed != 1) {
                token = -SENAO_CODE;
                tokenOK = -1;
            }

            if(tokenOK == 1) {
                token = nextToken;
                lex(token, arq);

                if(token != INICIO_CODE) {
                    checkSintaxe(token, arq);
                    remLastFuncion();
                } else {
                    checkSintaxe(token, arq);
                }
            }
        break;

        case REPITA_CODE:
            tokenOK = 1;

            if(nextToken == ATE_CODE) {
                token = -ATE_CODE;
                tokenOK = -1;
            }
        break;

        case ATE_CODE:
            tokenOK = 1;

            if(pilhaFuncions[quantFuncions-1].tipo != REPITA_CODE) {
                token = -ATE_CODE;
                tokenOK = -1;
            }

            checkSintaxe(LOGICA_CODE, arq);

            if(tokenOK == 1) {
                if(token != SEMICOLON) {
                    token = -SEMICOLON;
                    tokenOK = -1;
                } else {
                    if(quantParenteses == 0)
                        remLastFuncion();
                    else if(quantParenteses > 0) {
                        token = -RIGHT_PAREN;
                        tokenOK = -1;
                    } else if(quantParenteses < 0) {
                        token = -LEFT_PAREN;
                        tokenOK = -1;
                    }
                }
            }
        break;

        case CASO_CODE:
            tokenOK = 1;

            if(nextToken != IDENT) {
                token = -token;
                tokenOK = -1;
            } else if(varExiste(lexeme) == NULL) {
                token = -VAR_CODE;
                tokenOK = -1;
            } else {
                if(varExiste(lexeme)->tipo != INT_CODE) {
                    token = -token;
                    tokenOK = -1;
                    break;
                }

                token = nextToken;
                if(lex(tokenExt, arq) != ESCOLHA_CODE) {
                    token = -token;
                    tokenOK = -1;
                } else {
                    token = nextToken;
                    if(lex(tokenExt, arq) != INICIO_CODE) {
                        token = -CASO_CODE;
                        tokenOK = -1;
                    } else {
                        quantInicios++;
                        do {
                            token = nextToken;

                            if(lex(tokenExt, arq) != INT_LIT) {
                                if(nextToken != FIM_CODE) {
                                    token = -CASO_CODE;
                                    tokenOK = -1;
                                }
                            } else {
                                addLastFuncion(lexeme, nextToken);
                                token = nextToken;

                                if(lex(tokenExt, arq) != TWO_POINTS) {
                                    token = -TWO_POINTS;
                                    tokenOK = -1;
                                } else {
                                    token = nextToken;

                                    if(lex(tokenExt, arq) == INICIO_CODE) {
                                        do {
                                            token = nextToken;
                                            lex(tokenExt, arq);

                                            checkSintaxe(token, arq);

                                        } while((nextToken != FIM_CODE || pilhaFuncions[quantFuncions-1].tipo != INT_LIT) && tokenOK == 1);

                                        if(tokenOK == 1) {
                                            token = nextToken;
                                            lex(tokenExt, arq);

                                            checkSintaxe(token, arq);
                                        }
                                    } else {
                                        token = nextToken;
                                        lex(tokenExt, arq);

                                        checkSintaxe(token, arq);

                                        if(tokenOK == 1)
                                            remLastFuncion();
                                    }
                                }
                            }
                        } while(nextToken != FIM_CODE && tokenOK == 1);

                        if(tokenOK == 1) {
                            token = nextToken;
                            lex(tokenExt, arq);

                            checkSintaxe(token, arq);
                        }
                    }
                }
            }
        break;

        case IDENT:
            if(varExiste(lexeme) == NULL) {
                token = -VAR_CODE;
                tokenOK = -1;

            } else {
                if(nextToken == TWO_POINTS) {
                    token = nextToken;
                    if(lex(tokenExt, arq) == ASSIGN_OP) {
                        tokenOK = 1;

                        do {
                            checkSintaxe(OP_CODE, arq);
                        } while((nextToken == IDENT || nextToken == RIGHT_PAREN) && tokenOK == 1);
                    } else {
                        token = -ASSIGN_OP;
                        tokenOK = -1;
                    }
                } else {
                    token = -TWO_POINTS;
                    tokenOK = -1;
                }

                if(tokenOK != -1) {
                    if(quantParenteses > 0) {
                        token = -RIGHT_PAREN;
                        tokenOK = -1;
                    } else if(token == SEMICOLON) {
                        tokenOK = 1;
                    } else {
                        token = -SEMICOLON;
                        tokenOK = -1;
                    }
                }
            }
        break;

        case OP_CODE:
            tokenOK = 1;

            if(nextToken == IDENT && varExiste(lastIdent) == NULL) {
                token = -VAR_CODE;
                tokenOK = -1;
            } else if(isReservedWord() != IDENT) {
                tokenOK = 0;
            }

            if(tokenOK != 1) break;

            do {
                token = nextToken;
                lex(tokenExt, arq);

                if(nextToken == LEFT_PAREN) quantParenteses++;

                else if(nextToken >= ADD_OP && nextToken <= DIV_OP) {
                    token = nextToken;
                    if(lex(token, arq) != IDENT && nextToken != INT_LIT && nextToken != REAL_LIT) {
                        token = -token;
                        tokenOK = -1;

                        break;
                    }
                } else if((isReservedWord() == MOD_OP && token != INT_LIT) ||
                          (token == MOD_OP && nextToken != INT_LIT)) {
                    if(varExiste(lastIdent)->tipo != INT_CODE || (token != IDENT && nextToken != IDENT)) {
                        token = -MOD_OP;
                        tokenOK = -1;

                        break;
                    }
                } else if(nextToken == RIGHT_PAREN) {
                    quantParenteses--;
                    break;
                } else if(nextToken == ASSIGN_OP || nextToken == LOG_OP)
                    break;

                else if(isReservedWord() != IDENT) {
                    tokenOK = 0;

                    break;

                } else if(token == nextToken || (token == IDENT && nextToken == INT_LIT) ||
                          (token == INT_LIT && nextToken == IDENT)) {
                    token = -token;
                    tokenOK = -1;

                    break;
                } else if(nextToken == UNKNOWN) {
                    token = -token;
                    tokenOK = -1;

                    break;
                }

                if(quantParenteses < 0) {
                    token = -LEFT_PAREN;
                    tokenOK = -1;

                    break;
                }

            } while(nextToken != IDENT && nextToken != SEMICOLON);
        break;

        case LOGICA_CODE:
            tokenOK = 1;

            if(nextToken == IDENT && varExiste(lastIdent) == NULL) {
                token = -VAR_CODE;
                tokenOK = -1;
            } else if(isReservedWord() != IDENT) {
                tokenOK = 0;
            } else if(nextToken == LEFT_PAREN) {
                quantParenteses++;

                token = nextToken;

                if(lex(tokenExt, arq) == LEFT_PAREN) {
                    checkSintaxe(LOGICA_CODE, arq);

                    break;
                } else {
                    token = nextToken;
                    if(lex(tokenExt, arq) >= ADD_OP && nextToken <= DIV_OP) {
                        do {
                            checkSintaxe(OP_CODE, arq);
                        } while(nextToken != RIGHT_PAREN && tokenOK == 1);

                        if(tokenOK == 1) {
                            token = nextToken;
                            while(lex(tokenExt, arq) == RIGHT_PAREN) {
                                quantParenteses--;

                                token = nextToken;
                            }
                        }
                    }
                }
            }

            if(tokenOK != 1) break;

            if(nextToken < LOG_OP || nextToken > DIV_OP) {
                token = nextToken;
                lex(tokenExt, arq);
            }

            switch(nextToken) {
                case ASSIGN_OP:
                case LOG_OP:
                    token = nextToken;
                    lex(tokenExt, arq);

                    if(token == ASSIGN_OP) {
                        if(nextToken == ASSIGN_OP || nextToken == LOG_OP) {
                            tokenOK = 0;
                            break;
                        }
                    }

                    switch(nextToken) {
                        case ASSIGN_OP:
                        case LOG_OP:
                            token = nextToken;
                            if(lex(tokenExt, arq) != IDENT) break;

                        case IDENT:
                            if(varExiste(lastIdent) == NULL) {
                                token = -VAR_CODE;
                                tokenOK = -1;
                            }

                        case INT_LIT:
                        case REAL_LIT:
                            tokenOK = 1;

                        break;

                        case LEFT_PAREN:
                            checkSintaxe(OP_CODE, arq);
                        break;

                        default:
                            token = -LOGICA_CODE;
                            tokenOK = -1;
                    }
                break;

                default:
                    token = -LOGICA_CODE;
                    tokenOK = -1;
            }

            if(tokenOK == 1) {
                token = nextToken;
                while(lex(tokenExt, arq) == RIGHT_PAREN) {
                    quantParenteses--;

                    token = nextToken;
                }
            }
        break;

        case ESCOLHA_CODE:
        case FACA_CODE:
        case ENTAO_CODE:
            token = -token;
            tokenOK = -1;
        break;

        default:
            quantOPAfterClosed--;
            tokenOK = 1;
    }

    if(tokenOK != -1)
        token = (tokenOK == 1 ? nextToken : -tokenExt);

    tokenExt = (token > EOF ? token : EOF);

    return tokenExt;
}*/

void resultadoSintaxe() {
    if(tokenOK == 1) {
        if(quantInicios > 0) {
            token = -INICIO_CODE;
            tokenOK = -1;
        } else if(quantFuncions > 0) {
            token = -UNKNOWN;
            tokenOK = -1;
        }
    }

    printf("\nCódigo da análise: %d\n", tokenOK);

    if(nextToken != token) {
        printf("\nErro de sintaxe detectado! Código do erro %d\n", token);
        if(tokenOK == -1) {
            switch(token) {
                case -DOT:
                    printf("Era esperado \".\" após o lexema \"fim\"!\n");
                break;

                case -SEMICOLON:
                    printf("Era esperado \";\" antes de \"%s\"!\n", lexeme);
                break;

                case -TWO_POINTS:
                    printf("Era esperado \":\" antes de \"%s\"!\n", lexeme);
                break;

                case -INICIO_CODE:
                    printf("Era esperado \"fim\" antes de \"%s\"!\n", lexeme);
                break;

                case -VAR_CODE:
                    printf("Variável \"%s\" não declarada!\n", lastIdent);
                break;

                case -RIGHT_PAREN:
                    printf("Está faltando parenteses a direita!\n");
                break;

                case -LEFT_PAREN:
                    printf("Está sobrando parenteses a direita!\n");
                break;

                case -FIM_CODE:
                    printf("Está sobrando lexema \"fim\"!\n");
                break;

                case -MOD_OP:
                    printf("Operação de resto é inválida para a expressão definida!\n");
                break;

                case -LOGICA_CODE:
                    printf("Erro na construção do bloco de operações lógicas\n");
                break;

                case -SENAO_CODE:
                    printf("O lexema \"senao\" precisa vir logo após o término de uma operação \"se\" anterior!\n");
                break;

                case -ATE_CODE:
                    printf("O lexema \"ate\" precisa vir depois de um lexema \"repita\" anterior e um conjunto de instruções intermediárias!\n");
                break;

                case -ESCOLHA_CODE:
                    printf("Era esperado \"caso\" antes de \"escolha\"!\n");
                break;

                case -CASO_CODE:
                    printf("Erro na estruturação da função \"caso\"!\n");
                break;

                case -UNKNOWN:
                    printf("Erro desconhecido! Ainda existem funções na pilha após o término na análise!\n");
                break;

                default:
                    tokenOK = 0;
            }
        }

        if(tokenOK == 0)
            printf("O lexema \"%s\" com token %d não pode vir depois de um lexema com token %d\n", lexeme, nextToken, (-token));
    } else
        printf("\nCódigo completamente verificado!\n");
}

void addChar() {
    char *aux = lexeme;

    lexeme = new char [lexLen+2];

    if(aux != NULL) {
        strcpy(lexeme, aux);
        delete[] aux;
    }

    lexeme[lexLen++] = nextChar;
    lexeme[lexLen] = '\0';
}

void getChar(FILE *arq) {
    if((nextChar = getc(arq)) != EOF) {
        if(isalpha(nextChar))
            charClass = LETTER;

        else if(isdigit(nextChar))
            charClass = DIGIT;

        else
            charClass = UNKNOWN;
    } else
        charClass = EOF;
}

void getNonBlank(FILE *arq) {
    while(isspace(nextChar))
        getChar(arq);
}

int lex(int token, FILE *arq) {
    lexLen = 0;
    varType = 0;

    if(token != EOF) {
        if(lexeme != NULL) {
            if(lexeme != lastIdent && lexeme != nomePrograma && lexeme != lastFunction.nome) {
                if(quantFuncions > 0) {
                    if(lexeme != pilhaFuncions[quantFuncions-1].nome)
                        delete[] lexeme;
                } else
                    delete[] lexeme;
            }
        }

    } else {
        tokenOK = 1;
        nVars = 0;
        quantInicios = 0;
        quantParenteses = 0;
        quantFuncions = 0;
        quantOPAfterClosed = 0;
        declaredVars = NULL;
        lastIdent = NULL;
        lastFunction.nome = NULL;
        lastFunction.tipo = UNKNOWN;
        lastClosed.nome = NULL;
        lastClosed.tipo = UNKNOWN;
    }

    lexeme = NULL;

    getNonBlank(arq);

    switch(charClass) {
        case LETTER:
            do {
                addChar();
                getChar(arq);
            } while(charClass == LETTER || charClass == DIGIT);
            nextToken = isReservedWord();

            if(nextToken == IDENT) {
                if(lastIdent != NULL) delete[] lastIdent;

                lastIdent = lexeme;
            } else if(nextToken != ENTAO_CODE && nextToken != FACA_CODE &&
                      nextToken != INICIO_CODE && nextToken != FIM_CODE &&
                      nextToken != ESCOLHA_CODE && nextToken != ATE_CODE) {
                if(lastFunction.nome != NULL) delete[] lastFunction.nome;

                lastFunction.nome = lexeme;
                lastFunction.tipo = nextToken;
            }

        break;

        case DIGIT:
            do {
                addChar();
                getChar(arq);
            } while(charClass == DIGIT);

            nextToken = lookup(nextChar);

            if(nextToken == DOT || nextToken == COMMA) {
                addChar();
                getChar(arq);

                if(charClass == DIGIT) {
                    do {
                        addChar();
                        getChar(arq);
                    } while(charClass == DIGIT);
                    nextToken = REAL_LIT;
                } else {
                    nextToken = UNKNOWN;
                }
            } else
                nextToken = INT_LIT;

        break;

        case EOF:
            nextToken = EOF;

            if(lexeme != NULL)
                delete[] lexeme;

            lexeme = new char [4];

            strcpy(lexeme, "EOF");
        break;

        default:
            addChar();
            nextToken = lookup(nextChar);
            getChar(arq);
    }

    for(int i = 0; i < quantFuncions; i++) printf("\t");

    printf("Next token is: %d Next lexeme is %s\n", nextToken, lexeme);

    return nextToken;
}
