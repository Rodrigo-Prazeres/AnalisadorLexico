#ifndef TS_H
#define TS_H

#include "../include/token.h"

#define TABLE_SIZE 101 

typedef struct Simbolo {
    char lexema[100];
    TokenType tipo;
    struct Simbolo* prox;
} Simbolo;

void inicializarTS();
void inserirSimbolo(char* lexema, TokenType tipo);
Simbolo* buscarSimbolo(char* lexema);
int ehPalavraReservada(char* lexema);
void imprimirTS();
void gravarTS(char* nomeArquivo);

#endif