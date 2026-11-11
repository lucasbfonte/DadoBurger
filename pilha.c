#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

void pilhaInicializa(PilhaEnc *p) {
    p->topo = NULL;
}

int pilhaEstaVazia(PilhaEnc *p) {
    return p->topo == NULL;
}

int pilhaEmpilha(PilhaEnc *p, Ingrediente valor) {
    nodoPilha *novo = (nodoPilha*) malloc(sizeof(nodoPilha));
    if (novo == NULL)
        return 0;

    novo->dado = valor;
    novo->prox = p->topo; // aponta para o antigo topo
    p->topo = novo;        // o novo nodo passa a ser o topo
    return 1;
}

int pilhaDesempilha(PilhaEnc *p, Ingrediente *valorRemovido) {
    nodoPilha *removido;

    if (pilhaEstaVazia(p)) // underflow
        return 0;

    removido = p->topo;
    *valorRemovido = removido->dado;
    p->topo = removido->prox;
    free(removido);
    return 1;
}

int pilhaTopo(PilhaEnc *p, Ingrediente *valor) {
    if (pilhaEstaVazia(p))
        return 0;

    *valor = p->topo->dado;
    return 1;
}

int pilhaTamanho(PilhaEnc *p) {
    nodoPilha *aux = p->topo;
    int contador = 0;

    while (aux != NULL) {
        contador++;
        aux = aux->prox;
    }

    return contador;
}

void pilhaEsvaziar(PilhaEnc *p) {
    nodoPilha *aux;
    while (!pilhaEstaVazia(p)) {
        aux = p->topo;
        p->topo = aux->prox;
        free(aux);
    }
}
