/* hash.h
 * Tabela hash usada como cache de pedidos (codigo -> receita).
 * Base: codigo da aula 20 (encadeamento separado), adaptado.
 *
 * Adaptacoes em relacao ao codigo da aula (justificar no relatorio):
 *  1. Fica so o encadeamento separado (politica recomendada no enunciado).
 *     Sondagem linear, quadratica, hash duplo, hashString e
 *     hashMultiplicativo saem: o jogo nao usa.
 *  2. O valor guardado e a receita (string) em vez de um int.
 *  3. A struct ganhou as metricas exigidas: hits, misses e colisoes.
 *     Colisao = insercao em um slot que ja tinha pelo menos 1 entrada
 *     (conta 1 por insercao, nao importa o tamanho da lista).
 *  4. Buscar devolve a receita (ou NULL) e conta hit/miss.
 *  5. Inserir retorna 0 se faltar memoria (a aula nao checava malloc).
 *  6. Sai a remocao: um cache que so cresce durante a sessao nao remove.
 */

#ifndef HASH_H
#define HASH_H

#include "ingredientes.h"

typedef struct No {
    int        chave;                    /* codigo do pedido */
    char       valor[RECEITA_MAX + 1];   /* receita */
    struct No *prox;
} No;

typedef struct {
    No **tabela;   /* vetor de ponteiros para listas */
    int  m;        /* numero de slots */
    int  n;        /* numero de elementos */

    /* metricas */
    long hits;
    long misses;
    long colisoes;
} TabelaEnc;

/* Hash por divisao: h(k) = k mod m (m primo). */
int hashDivisao(int k, int m);

/* Retorna 0 se faltou memoria. */
int         encInicializar(TabelaEnc *t, int m);

/* Pre-condicao: a chave ainda nao esta na tabela (o cache so insere
 * depois de um miss). Retorna 0 se faltou memoria. */
int         encInserir(TabelaEnc *t, int chave, const char *valor);

/* Retorna a receita ou NULL. Conta hit ou miss. */
const char *encBuscar(TabelaEnc *t, int chave);

/* 1 se a chave esta na tabela. NAO conta hit/miss: e uma checagem
 * interna, nao uma consulta ao cache. */
int         encContem(const TabelaEnc *t, int chave);

double      encFatorCarga(const TabelaEnc *t);   /* lambda = n/m */
double      encTaxaAcerto(const TabelaEnc *t);   /* em %, 0 se sem buscas */

void        encDestruir(TabelaEnc *t);

#endif /* HASH_H */
