#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "token.h"

// Inicializa o parser recebendo o ponteiro do arquivo fonte
void iniciarAnaliseSintatica(FILE *arquivo);

// Funções do Parser (Não-terminais da gramática)
void programa();
void bloco();
void parteDeclaracoesVariaveis();
void declaracaoVariaveis();
void listaIdentificadores();
void tipo();
void comandoComposto();
void comando();
void atribuicao();
void comandoCondicional();
void comandoRepetitivo();
void expressao();
void relacao();
void expressaoSimples();
void termo();
void fator();

#endif