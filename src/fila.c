/* fila.c
 * Fila encadeada de clientes. Ver fila.h.
 */
#include <stdlib.h>

#include "fila.h"

void filaInicializar(FilaEnc *f) {
    f->ini = NULL;
    f->fim = NULL;
}

int filaVazia(const FilaEnc *f) {
    return f->ini == NULL;
}

int filaEnfileirar(FilaEnc *f, Cliente c) {
    nodoFila *novo = (nodoFila *) malloc(sizeof(nodoFila));
    if (novo == NULL)
        return 0;

    novo->dado = c;
    novo->prox = NULL;

    if (f->fim == NULL)      /* fila vazia: novo e inicio e fim */
        f->ini = novo;
    else
        f->fim->prox = novo; /* encadeia depois do ultimo */
    f->fim = novo;
    return 1;
}

int filaDesenfileirar(FilaEnc *f, Cliente *c) {
    nodoFila *removido;

    if (filaVazia(f))
        return 0;

    removido = f->ini;
    *c = removido->dado;
    f->ini = removido->prox;
    if (f->ini == NULL)      /* esvaziou: fim tambem fica nulo */
        f->fim = NULL;
    free(removido);
    return 1;
}

Cliente *filaFrente(FilaEnc *f) {
    return filaVazia(f) ? NULL : &f->ini->dado;
}

int filaTamanho(const FilaEnc *f) {
    int n = 0;
    for (const nodoFila *aux = f->ini; aux != NULL; aux = aux->prox)
        n++;
    return n;
}

void filaDestruir(FilaEnc *f) {
    Cliente lixo;
    while (filaDesenfileirar(f, &lixo))
        ;
}
