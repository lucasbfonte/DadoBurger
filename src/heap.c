/* heap.c
 * Heap minimo em vetor. Ver heap.h.
 */
#include <stdlib.h>

#include "heap.h"

#define PAI(i)  (((i) - 1) / 2)
#define ESQ(i)  (2 * (i) + 1)
#define DIR(i)  (2 * (i) + 2)

static void trocar(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/* Sobe o elemento i enquanto ele for menor que o pai. */
static void subir(HeapMin *h, int i) {
    while (i > 0 && h->v[i] < h->v[PAI(i)]) {
        trocar(&h->v[i], &h->v[PAI(i)]);
        i = PAI(i);
    }
}

/* Desce o elemento i trocando com o menor filho ate virar heap. */
static void descer(HeapMin *h, int i) {
    for (;;) {
        int menor = i;
        int e = ESQ(i), d = DIR(i);
        if (e < h->n && h->v[e] < h->v[menor]) menor = e;
        if (d < h->n && h->v[d] < h->v[menor]) menor = d;
        if (menor == i)
            return;
        trocar(&h->v[i], &h->v[menor]);
        i = menor;
    }
}

int heapIniciar(HeapMin *h, int cap_inicial) {
    if (cap_inicial < 1) cap_inicial = 1;
    h->v = (int *) malloc(cap_inicial * sizeof(int));
    h->n = 0;
    h->cap = h->v ? cap_inicial : 0;
    return h->v != NULL;
}

void heapDestruir(HeapMin *h) {
    free(h->v);
    h->v = NULL;
    h->n = h->cap = 0;
}

int heapVazio(const HeapMin *h)   { return h->n == 0; }
int heapTamanho(const HeapMin *h) { return h->n; }

int heapInserir(HeapMin *h, int chave) {
    if (h->n == h->cap) {                      /* cheio: dobra o vetor */
        int nova = h->cap ? 2 * h->cap : 4;
        int *v = (int *) realloc(h->v, nova * sizeof(int));
        if (v == NULL)
            return 0;
        h->v = v;
        h->cap = nova;
    }
    h->v[h->n] = chave;                        /* entra no fim... */
    subir(h, h->n);                            /* ...e sobe ate o lugar */
    h->n++;
    return 1;
}

int heapTopo(const HeapMin *h, int *chave) {
    if (heapVazio(h))
        return 0;
    *chave = h->v[0];
    return 1;
}

int heapRemover(HeapMin *h, int *chave) {
    if (heapVazio(h))
        return 0;
    *chave = h->v[0];
    h->n--;
    h->v[0] = h->v[h->n];                      /* ultimo vai pra raiz... */
    descer(h, 0);                              /* ...e desce */
    return 1;
}
