#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "ts.h"

// Assinatura da função do scanner
Token proximoToken(FILE *arquivo);

int main() {
    // Abra o arquivo. Certifique-se de que o caminho bate com o seu sistema.
    FILE *arquivo = fopen("entradas/teste.pas", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo fonte. Verifique se a pasta 'entradas' existe.\n");
        return 1;
    }

    // A mágica começa aqui: inicializa as palavras-chave
    inicializarTS();

    Token t;
    printf("--- INICIANDO ANALISE LEXICA ---\n");
    
    // Roda até o final do arquivo
    do {
        t = proximoToken(arquivo);
        
        if (t.tipo != FIM_DE_ARQUIVO) {
            printf("<Token: %d, Lexema: '%s'> [Linha: %d, Coluna: %d]\n", 
                    t.tipo, t.lexema, t.linha, t.coluna);
        }
                
    } while (t.tipo != FIM_DE_ARQUIVO);

    // Mostra como a tabela de símbolos ficou ao final da execução
    imprimirTS(); 

    fclose(arquivo);
    return 0;
}