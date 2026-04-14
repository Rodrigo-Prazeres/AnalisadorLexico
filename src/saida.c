#include <stdio.h>
#include "../include/token.h"

void registrarToken(FILE* fLex, Token t) {
    fprintf(fLex, "[%d, %d] Token Tipo: %d | Lexema: \"%s\"\n", 
            t.linha, t.coluna, t.tipo, t.lexema);
}

void registrarErro(FILE* fErr, int linha, int coluna, char* msg, char* lexema) {
    fprintf(fErr, "ERRO LÉXICO [%d:%d]: %s -> \"%s\"\n", linha, coluna, msg, lexema);
}