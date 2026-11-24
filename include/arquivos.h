/*
  arquivos.h - leitura dos arquivos de entrada (pedidos e historico).
 
  So faz parsing: devolve os dados lidos e nao sabe nada de AVL ou de
  lista contigua. Quem chama decide onde guardar.
 */
#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <stdio.h>

#include "ingredientes.h"

typedef enum {
    LEITURA_OK,
    LEITURA_FIM,     /* acabou o arquivo */
    LEITURA_ERRO     /* linha mal formatada */
} ResultadoLeitura;

/* Le o proximo pedido (duas linhas: codigo e receita).
 * Em LEITURA_ERRO, *linha_erro recebe o numero da linha problematica. */
ResultadoLeitura ler_pedido(FILE *fp, int *codigo,
                            char receita[RECEITA_MAX + 1],
                            long *linha_erro);

#define NOME_MAX 20   /* nome do jogador no historico: ate 20 caracteres */

/* Le a proxima entrada do historico (duas linhas: nome e pontuacao).
 * Nomes maiores que NOME_MAX sao truncados. */
ResultadoLeitura ler_historico(FILE *fp, char nome[NOME_MAX + 1],
                               int *pontos, long *linha_erro);

/* Acrescenta uma entrada no fim do arquivo (modo "a"), preservando as
 * anteriores. Retorna 0 se nao conseguiu abrir/gravar. */
int gravar_historico(const char *arquivo, const char *nome, int pontos);

#endif
