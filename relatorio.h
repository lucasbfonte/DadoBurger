/* relatorio.h
 * Escrita das metricas (secao 5) no formato do Apendice B.
 * Cada funcao recebe o FILE* de destino: a mesma funcao serve para a
 * tela (stdout) e para o medicoes.log, sem duplicar codigo.
 */
#ifndef RELATORIO_H
#define RELATORIO_H

#include <stdio.h>

#include "pedidos.h"

void relatorioCabecalho(FILE *out);

/* Secao 5.1: logo apos carregar os pedidos. */
void relatorioCargaAvl(FILE *out, const Pedidos *p);

/* Secao 5.2, itens 1 e 2: ao encerrar a partida. */
void relatorioBuscasAvl(FILE *out, const Pedidos *p);
void relatorioHash(FILE *out, const Pedidos *p);

#endif /* RELATORIO_H */
