/* fila.h
 * Fila encadeada (FIFO) dos clientes (secao 4.6).
 * Base: codigo da aula (FilaEnc com ponteiros de inicio e fim).
 *
 * Adaptacoes em relacao ao codigo da aula (justificar no relatorio):
 *  1. O dado guardado e um Cliente em vez de um int.
 *  2. filaFrente devolve um PONTEIRO para o cliente da frente, para a
 *     partida poder decrementar a paciencia sem desenfileirar.
 *  3. Nova filaDestruir: libera tudo ao fim da partida (inclusive se o
 *     jogador perder com clientes ainda na fila).
 */
#ifndef FILAENC_H
#define FILAENC_H

#include "ingredientes.h"

typedef struct {
    int  id;                       /* 1, 2, 3... ordem de chegada */
    int  paciencia;                /* turnos restantes */
    int  codigo;                   /* pedido sorteado (vale so na frente) */
    char receita[RECEITA_MAX + 1]; /* "" enquanto nao chegou na frente */
} Cliente;

typedef struct str_nodoFila nodoFila;

struct str_nodoFila {
    Cliente   dado;
    nodoFila *prox;
};

typedef struct {
    nodoFila *ini;
    nodoFila *fim;
} FilaEnc;

void     filaInicializar(FilaEnc *f);
int      filaVazia(const FilaEnc *f);
int      filaEnfileirar(FilaEnc *f, Cliente c);       /* 0 se faltou memoria */
int      filaDesenfileirar(FilaEnc *f, Cliente *c);   /* 0 se vazia */
Cliente *filaFrente(FilaEnc *f);                      /* NULL se vazia */
int      filaTamanho(const FilaEnc *f);
void     filaDestruir(FilaEnc *f);

#endif
