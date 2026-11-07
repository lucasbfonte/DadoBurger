/* avl.c
 * Arvore AVL dos pedidos. Ver avl.h para a lista de adaptacoes.
 */

#include <stdlib.h>
#include <string.h>

#include "avl.h"

/* ------------------------------------------------------------------ */
/*  Auxiliares internos                                                 */
/* ------------------------------------------------------------------ */
static int maximo(int a, int b){
  return a > b ? a : b;
}

/* Convencao do enunciado: vazia = 0. */
static int altura(const NodoAvl *n) { return n ? n->altura : 0; }

/* FB = h(esq) - h(dir). Positivo: pesado a esquerda. */
static int fb(const NodoAvl *n) {
    return n ? altura(n->esq) - altura(n->dir) : 0;
}

static NodoAvl *criarNodo(int codigo, const char *receita) {
    NodoAvl *n = malloc(sizeof(NodoAvl));
    if (!n) return NULL;
    n->codigo = codigo;
    strncpy(n->receita, receita, RECEITA_MAX);
    n->receita[RECEITA_MAX] = '\0';
    n->altura = 1;                 /* folha = 1 (aula usava 0) */
    n->esq = n->dir = NULL;
    return n;
}

static void atualizarAltura(NodoAvl *n) {
    if (n) n->altura = 1 + maximo(altura(n->esq), altura(n->dir));
}

/* ------------------------------------------------------------------ */
/*  Rotacoes simples (iguais a aula + contador)                         */
/* ------------------------------------------------------------------ */

/*       y                x
 *      / \             /   \
 *     x   T3   ->    T1     y
 *    / \                   / \
 *   T1  T2               T2   T3      */
static NodoAvl *rotDir(Avl *a, NodoAvl *y) {
    NodoAvl *x  = y->esq;
    NodoAvl *T2 = x->dir;
    x->dir = y;
    y->esq = T2;
    atualizarAltura(y);
    atualizarAltura(x);
    a->rotacoes++;
    return x;
}

/*     x                  y
 *    / \               /   \
 *   T1   y    ->      x     T3
 *       / \          / \
 *      T2  T3       T1  T2            */
static NodoAvl *rotEsq(Avl *a, NodoAvl *x) {
    NodoAvl *y  = x->dir;
    NodoAvl *T2 = y->esq;
    y->esq = x;
    x->dir = T2;
    atualizarAltura(x);
    atualizarAltura(y);
    a->rotacoes++;
    return y;
}

/* ------------------------------------------------------------------ */
/*  Reequilibrio (mesma logica da aula)                                 */
/* ------------------------------------------------------------------ */
static NodoAvl *reequilibrar(Avl *a, NodoAvl *raiz) {
    atualizarAltura(raiz);
    int f = fb(raiz);

    if (f > 1 && fb(raiz->esq) >= 0)            /* LL */
        return rotDir(a, raiz);

    if (f > 1 && fb(raiz->esq) < 0) {           /* LR: conta 2 */
        raiz->esq = rotEsq(a, raiz->esq);
        return rotDir(a, raiz);
    }

    if (f < -1 && fb(raiz->dir) <= 0)           /* RR */
        return rotEsq(a, raiz);

    if (f < -1 && fb(raiz->dir) > 0) {          /* RL: conta 2 */
        raiz->dir = rotDir(a, raiz->dir);
        return rotEsq(a, raiz);
    }

    return raiz;
}

/* status: 1 inseriu, 0 duplicata, -1 sem memoria */
static NodoAvl *inserirRec(Avl *a, NodoAvl *raiz, int codigo,
                           const char *receita, int *status) {
    if (!raiz) {
        NodoAvl *novo = criarNodo(codigo, receita);
        *status = novo ? 1 : -1;
        return novo;
    }
    if (codigo < raiz->codigo)
        raiz->esq = inserirRec(a, raiz->esq, codigo, receita, status);
    else if (codigo > raiz->codigo)
        raiz->dir = inserirRec(a, raiz->dir, codigo, receita, status);
    else {
        *status = 0;               /* duplicata ignorada, como na aula */
        return raiz;
    }

    /* So reequilibra se algo foi inserido: em duplicata ou falha de
     * memoria a arvore nao mudou. */
    return *status == 1 ? reequilibrar(a, raiz) : raiz;
}

static void destruirRec(NodoAvl *n) {
    if (!n) return;
    destruirRec(n->esq);
    destruirRec(n->dir);
    free(n);
}

/* ------------------------------------------------------------------ */
/*  Operacoes publicas                                                  */
/* ------------------------------------------------------------------ */
void avlIniciar(Avl *a) {
    a->raiz = NULL;
    a->nos = a->rotacoes = 0;
    a->buscas = a->comparacoes = 0;
}

void avlDestruir(Avl *a) {
    destruirRec(a->raiz);
    avlIniciar(a);
}

int avlInserir(Avl *a, int codigo, const char *receita) {
    int status = -1;
    a->raiz = inserirRec(a, a->raiz, codigo, receita, &status);
    if (status == 1) a->nos++;
    return status;
}

/* Iterativa: cada nodo visitado = 1 comparacao com a chave procurada. */
const NodoAvl *avlBuscar(Avl *a, int codigo) {
    const NodoAvl *n = a->raiz;
    a->buscas++;
    while (n) {
        a->comparacoes++;
        if (codigo == n->codigo) return n;
        n = codigo < n->codigo ? n->esq : n->dir;
    }
    return NULL;
}

/* Desce como na busca normal, lembrando o ultimo nodo em que foi para a
 * esquerda: esse e o menor codigo maior que o procurado visto ate agora. */
const NodoAvl *avlBuscarTeto(Avl *a, int codigo) {
    const NodoAvl *n = a->raiz;
    const NodoAvl *teto = NULL;
    a->buscas++;
    while (n) {
        a->comparacoes++;
        if (codigo == n->codigo) return n;
        if (codigo < n->codigo) {
            teto = n;          /* candidato: maior que o procurado */
            n = n->esq;
        } else {
            n = n->dir;
        }
    }
    return teto;
}

int avlAltura(const Avl *a) { return altura(a->raiz); }

int avlMinimo(const Avl *a) {
    const NodoAvl *n = a->raiz;
    while (n->esq) n = n->esq;
    return n->codigo;
}

int avlMaximo(const Avl *a) {
    const NodoAvl *n = a->raiz;
    while (n->dir) n = n->dir;
    return n->codigo;
}
