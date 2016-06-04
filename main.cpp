#include "lexeme.h"

int main(int nArgs, char *sArg[]) {
    setlocale(LC_CTYPE, "");
    FILE *in_fp;
    int nArg = nArgs, nextToken, token, status = 0;
    char *nomeArq = NULL;

    do {
        if(nArgs == 1) {
            printf("Digite o nome do arquivo: ");
            nomeArq = obterNome(nomeArq, stdin);
        } else
            nomeArq = sArg[nArg-1];

        printf("\nCarregando arquivo %s...\n\n", nomeArq);

        in_fp = fopen(nomeArq, "r");

        if(in_fp == NULL) {
            printf("\nFalha ao carregar o arquivo %s!\n\n", nomeArq);
            status++;
        } else {
            token = EOF;
            getChar(in_fp);
            do {
                nextToken = lex(token, in_fp);
                token = checkSintaxe(nextToken, in_fp);
            } while(nextToken > EOF && token > EOF);

            fclose(in_fp);

            resultadoSintaxe();
        }
    } while(--nArg > 1);

    if(nArgs > 1) {
        if(status > 0) printf("\n%d de %d arquivos falharam ao carregar!\n", status, nArgs-1);
        else printf("\nTodos os arquivos foram carregados com sucesso!\n");
    } else {
        if(status > 0) printf("\nFalha ao carregar o arquivo %s!\n", nomeArq);
        else printf("\nArquivo carregado com sucesso!\n");
    }

    getc(stdin);

    return status;
}
