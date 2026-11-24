/* heap.h
 * Heap minimo usado como estoque de cada ingrediente perecivel (secao 4.7).
 * Base: codigo da aula (heap em vetor, filhos de i em 2i+1 e 2i+2).
 *
 * Adaptacoes em relacao ao codigo da aula (justificar no relatorio):
 *  1. Heap de MINIMO: a chave e o turno de vencimento, e a unidade que
 *     vence primeiro fica na raiz (indice 0).
 *  2. Vetor alocado dinamicamente e dobrado quando enche, porque o
 *     jogador pode preparar quantas unidades quiser.
 *  3. Retornos 0/1 em vez de imprimir mensagens de erro (modulo de
 *     dados nao imprime: ver regra do README).
 */
#ifndef HEAP_H
#define HEAP_H

typedef struct {
    int *v;          /* turnos de vencimento */
    int  n;          /* elementos no heap */
    int  cap;        /* capacidade do vetor */
} HeapMin;

int  heapIniciar(HeapMin *h, int cap_inicial);  /* 0 se faltou memoria */
void heapDestruir(HeapMin *h);

int  heapVazio(const HeapMin *h);
int  heapTamanho(const HeapMin *h);

int  heapInserir(HeapMin *h, int chave);        /* 0 se faltou memoria */
int  heapTopo(const HeapMin *h, int *chave);    /* 0 se vazio */
int  heapRemover(HeapMin *h, int *chave);       /* 0 se vazio */

#endif
