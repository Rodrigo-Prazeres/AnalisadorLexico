#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "ts.h"

Simbolo* tabela[TABLE_SIZE];

// Função hash simples
int hash(char* str) {
    int h = 0;
    for (int i = 0; str[i]; i++) {
        h = (h * 31 + tolower(str[i])) % TABLE_SIZE;
    }
    return h;
}

// converter pra minúsculo
void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Inicializar tabela
void inicializarTS() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        tabela[i] = NULL;
    }

    // Carregar palavras reservadas
    inserirSimbolo("program", KW_PROGRAM);
    inserirSimbolo("var", KW_VAR);
    inserirSimbolo("integer", KW_INTEGER);
    inserirSimbolo("real", KW_REAL);
    inserirSimbolo("begin", KW_BEGIN);
    inserirSimbolo("end", KW_END);
    inserirSimbolo("if", KW_IF);
    inserirSimbolo("then", KW_THEN);
    inserirSimbolo("else", KW_ELSE);
    inserirSimbolo("while", KW_WHILE);
    inserirSimbolo("do", KW_DO);
}

// Buscar símbolo
Simbolo* buscarSimbolo(char* lexema) {
    char temp[100];
    strcpy(temp, lexema);
    toLowerCase(temp);

    int idx = hash(temp);
    Simbolo* atual = tabela[idx];

    while (atual != NULL) {
        if (strcmp(atual->lexema, temp) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

// Inserir símbolo
void inserirSimbolo(char* lexema, TokenType tipo) {
    char temp[100];
    strcpy(temp, lexema);
    toLowerCase(temp);

    if (buscarSimbolo(temp) != NULL) return;

    int idx = hash(temp);

    Simbolo* novo = (Simbolo*) malloc(sizeof(Simbolo));
    strcpy(novo->lexema, temp);
    novo->tipo = tipo;

    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

// Verificar palavra reservada
int ehPalavraReservada(char* lexema) {
    Simbolo* s = buscarSimbolo(lexema);
    if (s == NULL) return 0;
    return (s->tipo >= KW_PROGRAM && s->tipo <= KW_DO);
}

// Debug
void imprimirTS() {
    printf("\nTabela de Simbolos:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Simbolo* atual = tabela[i];
        while (atual != NULL) {
            printf("%s -> %d\n", atual->lexema, atual->tipo);
            atual = atual->prox;
        }
    }
}