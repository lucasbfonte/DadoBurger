#include <stdio.h>
#include <stdlib.h>
#include "listaEnc.h"

void listaInicializar(ListaEnc *lista) {
    lista->ini = NULL;
}

int listaTamanho(const ListaEnc *lista) {
    nodoLista *aux = lista->ini;
    int contador = 0;

    while (aux != NULL) {
        contador++;
        aux = aux->prox;
    }

    return contador;
}

int listaInserirInicio(ListaEnc *lista, Ingrediente ing) {
    nodoLista *novo = (nodoLista*) malloc(sizeof(nodoLista));
    if (novo == NULL)
        return 0; // falha na alocação

    novo->dado = ing;
    novo->prox = lista->ini; // o novo nodo aponta para o antigo início
    lista->ini = novo;        // o novo nodo passa a ser o início

    return 1;
}

int listaInserirFim(ListaEnc *lista, Ingrediente ing) {
    nodoLista *novo;
    nodoLista *aux;

    novo = (nodoLista*) malloc(sizeof(nodoLista));
    if (novo == NULL)
        return 0;

    novo->dado = ing;
    novo->prox = NULL;

    if (lista->ini == NULL) { // lista vazia: o novo nodo é o único (e o início)
        lista->ini = novo;
    } else {
        aux = lista->ini;
        while (aux->prox != NULL) // percorre até o último nodo
            aux = aux->prox;
        aux->prox = novo;
    }

    return 1;
}

void listaDestruir(ListaEnc *lista) {
    nodoLista *ant;
    nodoLista *aux;
    aux = lista->ini;

    while (aux != NULL) {
        ant = aux;
        aux = aux->prox;
        free(ant);
    }

    lista->ini = NULL;
}
