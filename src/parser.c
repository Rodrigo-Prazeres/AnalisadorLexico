#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"
#include "../include/token.h"

extern Token proximoToken(FILE *arquivo); // Assinatura do seu scanner

FILE *arqFonte;
Token tokenAtual;

// Função auxiliar para buscar o próximo token
void obterToken() {
    tokenAtual = proximoToken(arqFonte);
}

// Dispara o erro léxico no formato exigido e encerra
void erroSintatico() {
    if (tokenAtual.tipo == FIM_DE_ARQUIVO) {
        printf("%d:fim de arquivo não esperado.\n", tokenAtual.linha);
    } else {
        printf("%d:token nao esperado [%s].\n", tokenAtual.linha, tokenAtual.lexema);
    }
    exit(1);
}

// O coração do Parser Descendente Recursivo
void casaToken(TokenType esperado) {
    if (tokenAtual.tipo == esperado) {
        obterToken();
    } else {
        erroSintatico();
    }
}

// Ponto de entrada do analisador sintático
void iniciarAnaliseSintatica(FILE *arquivo) {
    arqFonte = arquivo;
    obterToken(); // Lê o primeiro token
    programa();
    
    if (tokenAtual.tipo != FIM_DE_ARQUIVO) {
        erroSintatico();
    }
    printf("\nAnalise Sintatica concluida com sucesso!\n");
}

// Regra: <programa> ::= program <identificador>; <bloco>.
void programa() {
    printf("Aplicando regra: <programa> ::= program <identificador>; <bloco>.\n");
    casaToken(KW_PROGRAM);
    casaToken(ID);
    casaToken(SMB_SEM);
    bloco();
    casaToken(SMB_DOT);
}

// Regra: <bloco> ::= <parte de declarações de variáveis> <comando composto>
void bloco() {
    printf("Aplicando regra: <bloco> ::= <parte de declaracoes de variaveis> <comando composto>\n");
    parteDeclaracoesVariaveis();
    comandoComposto();
}

// Regra: <parte de declarações de variáveis> ::= { var <declaração de variáveis> {; <declaração de variáveis>}; }
void parteDeclaracoesVariaveis() {
    while (tokenAtual.tipo == KW_VAR) {
        printf("Aplicando regra: <parte de declaracoes de variaveis>\n");
        casaToken(KW_VAR);
        declaracaoVariaveis();
        
        while (tokenAtual.tipo == SMB_SEM) {
            casaToken(SMB_SEM);
            if (tokenAtual.tipo == ID) { // Se o proximo for ID, é outra declaracao
                declaracaoVariaveis();
            } else {
                break; // Apenas fechou a instrução anterior
            }
        }
    }
}

// Regra: <declaração de variáveis> ::= <lista de identificadores>: <tipo>
void declaracaoVariaveis() {
    printf("Aplicando regra: <declaracao de variaveis> ::= <lista de identificadores>: <tipo>\n");
    listaIdentificadores();
    casaToken(SMB_COL); // Dois pontos (:)
    tipo();
}

// Regra: <lista de identificadores> ::= <identificador> {, <identificador>}
void listaIdentificadores() {
    printf("Aplicando regra: <lista de identificadores> ::= <identificador> {, <identificador>}\n");
    casaToken(ID);
    while (tokenAtual.tipo == SMB_COM) { // Virgula (,)
        casaToken(SMB_COM);
        casaToken(ID);
    }
}

// Regra: <tipo> ::= integer | real
void tipo() {
    printf("Aplicando regra: <tipo> ::= integer | real\n");
    if (tokenAtual.tipo == KW_INTEGER) {
        casaToken(KW_INTEGER);
    } else if (tokenAtual.tipo == KW_REAL) {
        casaToken(KW_REAL);
    } else {
        erroSintatico();
    }
}

// Regra: <comando composto> ::= begin <comando> { ; <comando> } end
void comandoComposto() {
    printf("Aplicando regra: <comando composto> ::= begin <comando> { ; <comando> } end\n");
    casaToken(KW_BEGIN);
    comando();
    while (tokenAtual.tipo == SMB_SEM) {
        casaToken(SMB_SEM);
        if (tokenAtual.tipo == KW_END) break; // Tratamento para ponto e virgula no final do bloco
        comando();
    }
    casaToken(KW_END);
}

// Regra: <comando> ::= <atribuição> | <comando composto> | <comando condicional> | <comando repetitivo>
void comando() {
    printf("Aplicando regra: <comando>\n");
    if (tokenAtual.tipo == ID) {
        atribuicao();
    } else if (tokenAtual.tipo == KW_BEGIN) {
        comandoComposto();
    } else if (tokenAtual.tipo == KW_IF) {
        comandoCondicional();
    } else if (tokenAtual.tipo == KW_WHILE) {
        comandoRepetitivo();
    } else {
        erroSintatico();
    }
}

// Regra: <atribuição> ::= <variável> := <expressão>
void atribuicao() {
    printf("Aplicando regra: <atribuicao> ::= <variavel> := <expressao>\n");
    casaToken(ID); // <variavel> é um ID
    casaToken(OP_ASS); // :=
    expressao();
}

// Regra: <comando condicional> ::= if <expressão> then <comando> [ else <comando> ]
void comandoCondicional() {
    printf("Aplicando regra: <comando condicional> ::= if <expressao> then <comando> [ else <comando> ]\n");
    casaToken(KW_IF);
    expressao();
    casaToken(KW_THEN);
    comando();
    if (tokenAtual.tipo == KW_ELSE) {
        casaToken(KW_ELSE);
        comando();
    }
}

// Regra: <comando repetitivo> ::= while <expressão> do <comando>
void comandoRepetitivo() {
    printf("Aplicando regra: <comando repetitivo> ::= while <expressao> do <comando>\n");
    casaToken(KW_WHILE);
    expressao();
    casaToken(KW_DO);
    comando();
}

// Regra: <expressão> ::= <expressão simples> [ <relação> <expressão simples> ]
void expressao() {
    printf("Aplicando regra: <expressao>\n");
    expressaoSimples();
    if (tokenAtual.tipo >= OP_EQ && tokenAtual.tipo <= OP_GE) { // Operadores relacionais
        relacao();
        expressaoSimples();
    }
}

// Regra: <relação> ::= = | <> | < | <= | >= | >
void relacao() {
    printf("Aplicando regra: <relacao>\n");
    if (tokenAtual.tipo >= OP_EQ && tokenAtual.tipo <= OP_GE) {
        casaToken(tokenAtual.tipo);
    } else {
        erroSintatico();
    }
}

// Regra: <expressão simples> ::= [+|-] <termo> { (+|-) <termo> }
void expressaoSimples() {
    printf("Aplicando regra: <expressao simples>\n");
    if (tokenAtual.tipo == OP_AD || tokenAtual.tipo == OP_MIN) {
        casaToken(tokenAtual.tipo);
    }
    termo();
    while (tokenAtual.tipo == OP_AD || tokenAtual.tipo == OP_MIN) {
        casaToken(tokenAtual.tipo);
        termo();
    }
}

// Regra: <termo> ::= <fator> { (*|/) <fator> }
void termo() {
    printf("Aplicando regra: <termo>\n");
    fator();
    while (tokenAtual.tipo == OP_MUL || tokenAtual.tipo == OP_DIV) {
        casaToken(tokenAtual.tipo);
        fator();
    }
}

// Regra: <fator> ::= <variável> | <número> | ( <expressão> )
void fator() {
    printf("Aplicando regra: <fator>\n");
    if (tokenAtual.tipo == ID) {
        casaToken(ID);
    } else if (tokenAtual.tipo == NUM_INT) {
        casaToken(NUM_INT);
    } else if (tokenAtual.tipo == NUM_REAL) {
        casaToken(NUM_REAL);
    } else if (tokenAtual.tipo == SMB_OPA) { // Abre parenteses '('
        casaToken(SMB_OPA);
        expressao();
        casaToken(SMB_CPA); // Fecha parenteses ')'
    } else {
        erroSintatico();
    }
}