#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"
#include "ts.h"

// Variáveis globais de controle de posição
int linha_atual = 1;
int coluna_atual = 1;

// Função auxiliar para ler o próximo caractere e atualizar a coluna
char ler_char(FILE *arquivo) {
    char c = fgetc(arquivo);
    if (c != EOF) coluna_atual++;
    return c;
}

// Função auxiliar para devolver o caractere e retroceder a coluna
void devolver_char(char c, FILE *arquivo) {
    if (c != EOF) {
        ungetc(c, arquivo);
        coluna_atual--;
    }
}

// O MOTOR DO ANALISADOR (100% COMPLETO)
Token proximoToken(FILE *arquivo) {
    Token token;
    int estado = 0;
    int pos = 0;
    char c;

    // Resetando o lexema
    memset(token.lexema, 0, sizeof(token.lexema));

    while (1) {
        c = ler_char(arquivo);

        switch (estado) {
            case 0: // ESTADO INICIAL
                // 1. Ignorar espaços em branco e quebras de linha
                if (isspace(c)) {
                    if (c == '\n') {
                        linha_atual++;
                        coluna_atual = 1;
                    }
                    continue; 
                }

                // 2. Fim de arquivo
                if (c == EOF) {
                    token.tipo = FIM_DE_ARQUIVO;
                    strcpy(token.lexema, "EOF");
                    return token;
                }

                // Gravamos a posição inicial do token
                token.linha = linha_atual;
                token.coluna = coluna_atual - 1; 

                // 3. Identificadores (Letra seguida de letras/dígitos)
                if (isalpha(c)) {
                    estado = 1;
                    token.lexema[pos++] = c;
                }
                // 4. Números (Inteiros ou Reais)
                else if (isdigit(c)) {
                    estado = 2;
                    token.lexema[pos++] = c;
                }
                // 5. Comentários { ... }
                else if (c == '{') {
                    estado = 10; 
                }
                // 6. Atribuição (:=) ou Dois Pontos (:)
                else if (c == ':') {
                    estado = 4;
                    token.lexema[pos++] = c;
                }
                // 7. Relacionais Menores (<, <=, <>)
                else if (c == '<') {
                    estado = 5;
                    token.lexema[pos++] = c;
                }
                // 8. Relacionais Maiores (>, >=)
                else if (c == '>') {
                    estado = 6;
                    token.lexema[pos++] = c;
                }
                // 9. Símbolos e Operadores Simples Diretos
                else if (c == ';') { token.tipo = SMB_SEM; token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == ',') { token.tipo = SMB_COM; token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '.') { token.tipo = SMB_DOT; token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '(') { token.tipo = SMB_OPA; token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == ')') { token.tipo = SMB_CPA; token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '=') { token.tipo = OP_EQ;   token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '+') { token.tipo = OP_AD;   token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '-') { token.tipo = OP_MIN;  token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '*') { token.tipo = OP_MUL;  token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                else if (c == '/') { token.tipo = OP_DIV;  token.lexema[0] = c; token.lexema[1] = '\0'; return token; }
                // 10. Caractere Inválido (ERRO LÉXICO)
                else {
                    token.tipo = ERRO;
                    token.lexema[0] = c;
                    token.lexema[1] = '\0';
                    return token;
                }
                break;

            case 1: // ESTADO: IDENTIFICADOR E PALAVRAS RESERVADAS
                if (isalnum(c)) {
                    token.lexema[pos++] = c;
                } else {
                    devolver_char(c, arquivo);
                    token.lexema[pos] = '\0'; // Finaliza a string

                    // Consulta na Tabela de Símbolos
                    Simbolo* s = buscarSimbolo(token.lexema);
                    if (s != NULL) {
                        token.tipo = s->tipo; // Já existe (Palavra reservada ou ID)
                    } else {
                        inserirSimbolo(token.lexema, ID); // É novo, insere
                        token.tipo = ID;
                    }
                    return token;
                }
                break;

            case 2: // ESTADO: NÚMERO INTEIRO
                if (isdigit(c)) {
                    token.lexema[pos++] = c;
                } else if (c == '.') {
                    estado = 3; // Vai para real
                    token.lexema[pos++] = c;
                } else {
                    devolver_char(c, arquivo);
                    token.lexema[pos] = '\0';
                    token.tipo = NUM_INT;
                    return token;
                }
                break;

            case 3: // ESTADO: NÚMERO REAL
                if (isdigit(c)) {
                    token.lexema[pos++] = c;
                } else {
                    devolver_char(c, arquivo);
                    token.lexema[pos] = '\0';
                    token.tipo = NUM_REAL;
                    return token;
                }
                break;

            case 4: // ESTADO: VERIFICA := OU :
                if (c == '=') {
                    token.lexema[pos++] = c;
                    token.lexema[pos] = '\0';
                    token.tipo = OP_ASS;
                } else {
                    devolver_char(c, arquivo);
                    token.lexema[pos] = '\0';
                    token.tipo = SMB_COL;
                }
                return token;

            case 5: // ESTADO: VERIFICA <, <= ou <>
                if (c == '=') {
                    token.lexema[pos++] = c;
                    token.lexema[pos] = '\0';
                    token.tipo = OP_LE;
                } else if (c == '>') {
                    token.lexema[pos++] = c;
                    token.lexema[pos] = '\0';
                    token.tipo = OP_NE;
                } else {
                    devolver_char(c, arquivo);
                    token.lexema[pos] = '\0';
                    token.tipo = OP_LT;
                }
                return token;

            case 6: // ESTADO: VERIFICA > ou >=
                if (c == '=') {
                    token.lexema[pos++] = c;
                    token.lexema[pos] = '\0';
                    token.tipo = OP_GE;
                } else {
                    devolver_char(c, arquivo);
                    token.lexema[pos] = '\0';
                    token.tipo = OP_GT;
                }
                return token;

            case 10: // ESTADO: COMENTÁRIO { ... }
                if (c == '}') {
                    estado = 0; // Fim do comentário, volta a ler token
                } else if (c == EOF) {
                    token.tipo = ERRO;
                    strcpy(token.lexema, "Comentario nao fechado");
                    token.linha = linha_atual;
                    token.coluna = coluna_atual;
                    return token;
                } else if (c == '\n') {
                    linha_atual++;
                    coluna_atual = 1;
                }
                break;
        }
    }
}