/*
  arquivos.h - leitura dos arquivos de entrada (pedidos e historico).
 
  So faz parsing: devolve os dados lidos e nao sabe nada de AVL ou de
  lista contigua. Quem chama decide onde guardar.
 */
#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <stdio.h>

#include "Ingredientes.h"

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

#endif
