/* pilha.h
 * Pilha encadeada usada na montagem do hamburguer (secao 4.5).
 * Base: codigo da aula (PilhaEnc), adaptado.
 *
 * Adaptacoes em relacao ao codigo da aula (justificar no relatorio):
 *  1. O dado guardado e um Ingrediente em vez de Produto.
 *  2. Funcoes ganharam o prefixo "pilha" (a lista tambem tinha
 *     inicializa/estaVazia, e os nomes colidiam).
 *  3. Nova pilhaEsvaziar: libera todos os nodos (troca de cliente).
 *  4. Nova pilhaVerifica: compara a pilha, de baixo para cima, com a
 *     lista encadeada da receita usando uma pilha auxiliar e devolvendo
 *     tudo ao final (opcao (b) da secao 4.5: a pilha nao e destruida).
 */
#ifndef PILHAENC_H
#define PILHAENC_H

#include "ingredientes.h"
#include "lista.h"

typedef struct str_nodoPilha nodoPilha;

struct str_nodoPilha {
    nodoPilha  *prox;
    Ingrediente dado;
};

typedef struct {
    nodoPilha *topo;
} PilhaEnc;

void pilhaInicializa(PilhaEnc *p);
int  pilhaEstaVazia(PilhaEnc *p);
int  pilhaEmpilha(PilhaEnc *p, Ingrediente valor);            /* 0 se faltou memoria */
int  pilhaDesempilha(PilhaEnc *p, Ingrediente *valorRemovido); /* 0 se vazia */
int  pilhaTopo(PilhaEnc *p, Ingrediente *valor);               /* 0 se vazia */
int  pilhaTamanho(PilhaEnc *p);
void pilhaEsvaziar(PilhaEnc *p);

/* 1 = bate com a receita, 0 = nao bate, -1 = erro de memoria. */
int  pilhaVerifica(PilhaEnc *p, const ListaEnc *receita);

#endif
