#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"
#include "lista.h"

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

// retorno: 1 = bate, 0 = não bate, -1 = erro de memória
int pilhaVerifica(PilhaEnc *p, const ListaEnc *receita) {
    PilhaEnc aux;
    Ingrediente ing;
    pilhaInicializa(&aux);

    while (pilhaDesempilha(p, &ing)) {
        if (!pilhaEmpilha(&aux, ing)) {
            pilhaEsvaziar(&aux);
            return -1;
        }
    }

    const nodoLista *no = receita->ini;
    int confere = 1;

    while (pilhaDesempilha(&aux, &ing)) {
        if (no != NULL) {
            if (ing != no->dado) {
                confere = 0;
            }
            no = no->prox; // avança na lista
        } else {
            confere = 0; // a lista acabou, mas ainda há ingredientes na pilha
        }
        if (!pilhaEmpilha(p, ing)) {
            pilhaEsvaziar(&aux);
            return -1; // falha na alocação
        }
    }

    if (no != NULL) {
        confere = 0;
    }
    return confere;
}