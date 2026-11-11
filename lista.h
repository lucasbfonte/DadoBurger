#ifndef LISTAENC_H
#include "ingredientes.h"
#define LISTAENC_H


typedef struct str_nodoLista nodoLista;

struct str_nodoLista {
    nodoLista *prox;
    Ingrediente dado;
};

typedef struct {
    nodoLista *ini;
} ListaEnc;

void    listaInicializar(ListaEnc *lista);
int     listaInserirInicio(ListaEnc *lista, Ingrediente ing);
int     listaInserirFim(ListaEnc *lista, Ingrediente ing);
void    listaDestruir(ListaEnc *lista);
int     listaTamanho(const ListaEnc *lista);

#endif
