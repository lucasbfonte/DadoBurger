/* historico.c
 * Lista contigua do historico + Merge Sort. Ver historico.h.
 *
 * Por que Merge Sort (justificar no relatorio):
 *  - O(n log n) no pior caso, sem o pior caso O(n^2) do Quicksort, que
 *    aparece justamente com dados ja ordenados ou com muitas chaves
 *    repetidas (o historico tem muitas pontuacoes iguais).
 *  - E estavel: ao ordenar por pontuacao, quem tem a mesma pontuacao
 *    mantem a ordem relativa. (Mesmo assim o criterio ja desempata.)
 *  - Custo: vetor auxiliar de n posicoes, aceitavel aqui.
 */
#include <stdlib.h>
#include <string.h>

#include "historico.h"
#include "timer.h"

int historicoIniciar(Historico *h) {
    h->n = 0;
    h->cap = 16;
    h->v = (Entrada *) malloc(h->cap * sizeof(Entrada));
    if (h->v == NULL) {
        h->cap = 0;
        return 0;
    }
    return 1;
}

void historicoDestruir(Historico *h) {
    free(h->v);
    h->v = NULL;
    h->n = h->cap = 0;
}

int historicoInserirFim(Historico *h, const char *nome, int pontos) {
    if (h->n == h->cap) {                      /* cheio: dobra */
        int nova = h->cap ? 2 * h->cap : 16;
        Entrada *v = (Entrada *) realloc(h->v, nova * sizeof(Entrada));
        if (v == NULL)
            return 0;
        h->v = v;
        h->cap = nova;
    }
    strncpy(h->v[h->n].nome, nome, NOME_MAX);
    h->v[h->n].nome[NOME_MAX] = '\0';
    h->v[h->n].pontos = pontos;
    h->n++;
    return 1;
}

int historicoCarregar(Historico *h, FILE *fp, long *linha_erro) {
    char nome[NOME_MAX + 1];
    int  pontos;
    ResultadoLeitura r;

    if (fp == NULL)
        return 1;

    while ((r = ler_historico(fp, nome, &pontos, linha_erro)) == LEITURA_OK)
        if (!historicoInserirFim(h, nome, pontos))
            return -1;

    return r == LEITURA_ERRO ? 0 : 1;
}

int historicoCopiar(Historico *dest, const Historico *orig) {
    if (dest->cap < orig->n) {
        Entrada *v = (Entrada *) realloc(dest->v, orig->n * sizeof(Entrada));
        if (v == NULL)
            return 0;
        dest->v = v;
        dest->cap = orig->n;
    }
    memcpy(dest->v, orig->v, orig->n * sizeof(Entrada));
    dest->n = orig->n;
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Merge Sort (base: codigo da aula)                                   */
/* ------------------------------------------------------------------ */

/* < 0 se a deve vir antes de b. */
static int comparar(const Entrada *a, const Entrada *b, CriterioOrdem c) {
    if (c == ORDEM_PONTUACAO) {
        if (a->pontos != b->pontos)
            return b->pontos - a->pontos;       /* decrescente */
        return strcmp(a->nome, b->nome);
    }
    int r = strcmp(a->nome, b->nome);
    if (r != 0)
        return r;
    return b->pontos - a->pontos;
}

/* Intercala v[ini..meio] e v[meio+1..fim] usando aux. */
static void intercalar(Entrada *v, Entrada *aux, int ini, int meio, int fim,
                       CriterioOrdem c) {
    int i = ini, j = meio + 1, k = ini;

    while (i <= meio && j <= fim) {
        /* <= mantem a estabilidade: em empate, o da esquerda vem antes */
        if (comparar(&v[i], &v[j], c) <= 0)
            aux[k++] = v[i++];
        else
            aux[k++] = v[j++];
    }
    while (i <= meio) aux[k++] = v[i++];
    while (j <= fim)  aux[k++] = v[j++];

    for (k = ini; k <= fim; k++)
        v[k] = aux[k];
}

static void mergeSort(Entrada *v, Entrada *aux, int ini, int fim,
                      CriterioOrdem c) {
    if (ini >= fim)
        return;
    int meio = ini + (fim - ini) / 2;
    mergeSort(v, aux, ini, meio, c);
    mergeSort(v, aux, meio + 1, fim, c);
    intercalar(v, aux, ini, meio, fim, c);
}

double historicoOrdenar(Historico *h, CriterioOrdem criterio) {
    if (h->n < 2)
        return 0.0;

    /* O vetor auxiliar e alocado fora do intervalo cronometrado, assim
     * como a copia (secao 5.2): mede-se so o algoritmo. */
    Entrada *aux = (Entrada *) malloc(h->n * sizeof(Entrada));
    if (aux == NULL)
        return -1.0;

    Timer t;
    timer_iniciar(&t);
    mergeSort(h->v, aux, 0, h->n - 1, criterio);
    double s = timer_parar(&t);

    free(aux);
    return s;
}

const char *historicoNomeCriterio(CriterioOrdem c) {
    return c == ORDEM_PONTUACAO ? "pontuacao decrescente" : "ordem alfabetica";
}
