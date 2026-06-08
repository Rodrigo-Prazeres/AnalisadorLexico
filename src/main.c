#include <stdio.h>
#include <stdlib.h>
#include "../include/token.h"
#include "../include/ts.h"
#include "../include/parser.h" // <- Importante incluir o parser

int main() {
    FILE *arquivo = fopen("entradas/teste.pas", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo fonte. Verifique se a pasta 'entradas' existe.\n");
        return 1;
    }

    inicializarTS();

    printf("--- INICIANDO COMPILACAO (LEXICA + SINTATICA) ---\n");
    
    // A chamada inicial dispara toda a cadeia de validação do código
    iniciarAnaliseSintatica(arquivo);

    imprimirTS(); 
    fclose(arquivo);
    gravarTS("saida.ts");
    
    printf("Compilacao finalizada com sucesso! Arquivo da TS gerado.\n");
    return 0;
}