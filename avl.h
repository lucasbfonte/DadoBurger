/* avl.h
 * Arvore AVL que armazena os pedidos do Dado Burger.
 * Base: codigo da aula (INF01203 - UFRGS), adaptado para o trabalho.
 *
 * Adaptacoes em relacao ao codigo da aula (justificar no relatorio):
 *  1. O nodo guarda um pedido (codigo + receita) em vez de um int solto.
 *     A chave de ordenacao continua sendo um int: o codigo do pedido.
 *  2. Altura segue a convencao do enunciado: vazia = 0, so a raiz = 1
 *     (a aula usava vazia = -1, folha = 0). O fator de balanceamento nao
 *     muda, porque e uma diferenca de alturas.
 *  3. A arvore fica dentro de uma struct Avl, que tambem carrega as
 *     metricas exigidas (nos, rotacoes, buscas, comparacoes). Assim nao
 *     ha variaveis globais e as metricas andam junto com a arvore.
 *  4. Rotacoes sao contadas dentro de rotDir/rotEsq. Como uma rotacao
 *     dupla chama as duas, ela conta como 2 automaticamente (convencao
 *     do enunciado).
 *  5. A busca virou iterativa e conta 1 comparacao por nodo visitado.
 *  6. Nova busca "teto" (menor codigo >= x): se o codigo sorteado nao
 *     existir, o jogo usa o proximo existente com UMA busca so, em vez
 *     de sortear de novo (ver pedidos.c).
 *  7. Saem: remocao (o jogo nunca remove pedidos), impressao em-ordem
 *     (modulos de dados nao imprimem) e o main de demonstracao.
 *     Entram: minimo/maximo, para sortear um codigo no intervalo valido.
 */

#ifndef AVL_H
#define AVL_H

#include "ingredientes.h"

typedef struct NodoAvl {
    int             codigo;                   /* chave */
    char            receita[RECEITA_MAX + 1];
    int             altura;
    struct NodoAvl *esq;
    struct NodoAvl *dir;
} NodoAvl;

typedef struct {
    NodoAvl *raiz;

    /* metricas de carregamento */
    long nos;          /* nos efetivamente inseridos (duplicatas nao contam) */
    long rotacoes;     /* rotacoes simples; dupla = 2 */

    /* metricas de busca */
    long buscas;
    long comparacoes;  /* 1 por nodo visitado */
} Avl;

void avlIniciar(Avl *a);
void avlDestruir(Avl *a);

/* Insere o pedido. Retorna 1 se inseriu, 0 se o codigo ja existia,
 * -1 se faltou memoria. */
int  avlInserir(Avl *a, int codigo, const char *receita);

/* Retorna o nodo com o codigo ou NULL. Atualiza buscas/comparacoes. */
const NodoAvl *avlBuscar(Avl *a, int codigo);

/* Retorna o nodo com o menor codigo >= 'codigo' (o "teto"), ou NULL se
 * todos forem menores. Se o codigo existe, devolve ele mesmo, com o
 * mesmo numero de comparacoes que avlBuscar. Atualiza buscas/comparacoes. */
const NodoAvl *avlBuscarTeto(Avl *a, int codigo);

/* Altura da arvore (vazia = 0). */
int  avlAltura(const Avl *a);

/* Menor e maior codigo. So chamar com a arvore nao vazia. */
int  avlMinimo(const Avl *a);
int  avlMaximo(const Avl *a);

#endif /* AVL_H */
