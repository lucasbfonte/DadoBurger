#ifndef LISTAENC_H
#define LISTAENC_H

#include "ingredientes.h"


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

/* Monta a lista a partir da string da receita ("nkmpz", de baixo para
 * cima). Retorna 1 se deu certo; em erro (simbolo invalido ou falta de
 * memoria) deixa a lista vazia e retorna 0. */
int     listaMontar(ListaEnc *lista, const char *receita);

#endif
