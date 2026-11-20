/* historico.h
 * Lista contigua com o historico de partidas (secao 4.2) e a ordenacao
 * cronometrada usada na tela de Historico (secao 5.2, item 3).
 * Base: lista contigua da aula, adaptada.
 *
 * Adaptacoes em relacao ao codigo da aula (justificar no relatorio):
 *  1. O elemento e uma Entrada (nome + pontuacao) em vez de um int.
 *  2. O vetor e alocado dinamicamente e dobra quando enche, porque o
 *     programa nao pode assumir o tamanho do arquivo (secao 6).
 *  3. So existe insercao no fim: e a unica que o jogo usa.
 *  4. historicoCopiar cria uma copia independente, que e o que a
 *     ordenacao recebe (o original e o arquivo nunca sao reordenados).
 */
#ifndef HISTORICO_H
#define HISTORICO_H

#include <stdio.h>

#include "arquivos.h"

typedef struct {
    char nome[NOME_MAX + 1];
    int  pontos;
} Entrada;

typedef struct {
    Entrada *v;
    int      n;
    int      cap;
} Historico;

typedef enum {
    ORDEM_ALFABETICA,     /* nome A-Z (empate: maior pontuacao primeiro) */
    ORDEM_PONTUACAO       /* pontuacao decrescente (empate: nome A-Z) */
} CriterioOrdem;

int  historicoIniciar(Historico *h);                       /* 0 se faltou memoria */
void historicoDestruir(Historico *h);
int  historicoInserirFim(Historico *h, const char *nome, int pontos);

/* Le o arquivo inteiro para a lista. fp pode ser NULL (arquivo nao
 * existe ainda): historico fica vazio. Retorna 1 ok, 0 erro de formato
 * (*linha_erro recebe a linha) ou -1 se faltou memoria. */
int  historicoCarregar(Historico *h, FILE *fp, long *linha_erro);

/* Copia independente (destino precisa estar iniciado). 0 se faltou memoria. */
int  historicoCopiar(Historico *dest, const Historico *orig);

/* Ordena h->v pelo criterio, com Merge Sort. Retorna o tempo em
 * segundos (so o algoritmo e cronometrado) ou -1 se faltou memoria
 * para o vetor auxiliar. */
double historicoOrdenar(Historico *h, CriterioOrdem criterio);

const char *historicoNomeCriterio(CriterioOrdem c);

#endif
