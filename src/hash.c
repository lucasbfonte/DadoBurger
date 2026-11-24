/* hash.c
 * Tabela hash com encadeamento separado. Ver hash.h para as adaptacoes.
 */

#include <stdlib.h>
#include <string.h>

#include "hash.h"

/* Igual a aula, mas protegida contra codigo negativo: em C, -7 % 5 = -2,
 * o que daria indice negativo. Os pedidos atuais sao positivos, mas o
 * arquivo final pode nao ser. */
int hashDivisao(int k, int m) {
    int r = k % m;
    return r < 0 ? r + m : r;
}

int encInicializar(TabelaEnc *t, int m) {
    t->m = m;
    t->n = 0;
    t->hits = t->misses = t->colisoes = 0;
    t->tabela = malloc(m * sizeof(No *));
    if (!t->tabela) return 0;
    for (int i = 0; i < m; i++)
        t->tabela[i] = NULL;
    return 1;
}

int encInserir(TabelaEnc *t, int chave, const char *valor) {
    int i    = hashDivisao(chave, t->m);
    No *novo = malloc(sizeof(No));
    if (!novo) return 0;

    if (t->tabela[i] != NULL)        /* slot ja ocupado: colisao */
        t->colisoes++;

    novo->chave = chave;
    strncpy(novo->valor, valor, RECEITA_MAX);
    novo->valor[RECEITA_MAX] = '\0';
    novo->prox   = t->tabela[i];     /* insere no inicio, como na aula */
    t->tabela[i] = novo;
    t->n++;
    return 1;
}

const char *encBuscar(TabelaEnc *t, int chave) {
    int i = hashDivisao(chave, t->m);
    for (No *p = t->tabela[i]; p != NULL; p = p->prox) {
        if (p->chave == chave) {
            t->hits++;
            return p->valor;
        }
    }
    t->misses++;
    return NULL;
}

int encContem(const TabelaEnc *t, int chave) {
    int i = hashDivisao(chave, t->m);
    for (const No *p = t->tabela[i]; p != NULL; p = p->prox)
        if (p->chave == chave) return 1;
    return 0;
}

double encFatorCarga(const TabelaEnc *t) {
    return (double)t->n / t->m;
}

double encTaxaAcerto(const TabelaEnc *t) {
    long total = t->hits + t->misses;
    return total ? 100.0 * t->hits / total : 0.0;
}

void encDestruir(TabelaEnc *t) {
    for (int i = 0; i < t->m; i++) {
        No *p = t->tabela[i];
        while (p) {
            No *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(t->tabela);
    t->tabela = NULL;
    t->m = t->n = 0;
}
