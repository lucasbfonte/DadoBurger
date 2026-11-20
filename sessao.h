/* sessao.h
 * Tudo que vive durante a execucao inteira do programa: pedidos (AVL +
 * cache), historico (lista contigua) e o medicoes.log.
 *
 * As duas interfaces (texto e raylib) so falam com este modulo e com a
 * partida, entao nenhuma delas repete a logica de carregar arquivos,
 * gravar historico ou escrever metricas.
 */
#ifndef SESSAO_H
#define SESSAO_H

#include <stdio.h>

#include "historico.h"
#include "partida.h"
#include "pedidos.h"

#define ARQ_LOG "medicoes.log"

typedef struct {
    Pedidos     pedidos;
    Historico   historico;
    FILE       *log;
    const char *arq_historico;      /* argv[2]: onde gravar novas partidas */
    int         historico_existia;  /* 0 = arquivo nao existia ao abrir */
    int         historico_carregado;/* entradas lidas do arquivo (metrica 5.1) */
} Sessao;

/* Carrega pedidos (argv[1]) e historico (argv[2]), cria o medicoes.log
 * e grava as metricas da secao 5.1. Em erro, escreve uma mensagem em
 * 'erro' e retorna 0 (a sessao ja fica liberada). */
int  sessaoIniciar(Sessao *s, const char *arq_pedidos,
                   const char *arq_historico, char *erro, int tam_erro);
void sessaoEncerrar(Sessao *s);

/* Nova partida com os parametros do config.h. */
int  sessaoNovaPartida(Sessao *s, Partida *p);

/* Fim de partida (vitoria, derrota ou desistencia): acrescenta a entrada
 * no fim da lista e no arquivo, e grava o Relatorio de Desempenho no log.
 * Retorna 0 se nao conseguiu gravar o historico. */
int  sessaoFimDePartida(Sessao *s, const char *nome, int pontos);

/* Escreve o Relatorio de Desempenho completo em 'out'. */
void sessaoRelatorio(Sessao *s, FILE *out);

/* Copia o historico para 'copia' (fora do tempo), ordena com Merge Sort
 * (cronometrado) e grava a medicao no log. Retorna o tempo, ou -1 se
 * faltou memoria. 'copia' precisa ter sido iniciada. */
double sessaoOrdenarHistorico(Sessao *s, Historico *copia, CriterioOrdem c);

#endif /* SESSAO_H */
