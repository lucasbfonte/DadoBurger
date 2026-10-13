/*
 * Dado Burger - INF01203 (UFRGS)
 * Ponto de entrada: le os nomes dos arquivos via argv e prepara o programa.
 *
 * Uso: ./dado_burger <pedidos> <historico>
 */
#include <stdio.h>
#include <stdlib.h>

#include "timer.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <pedidos> <historico>\n", argv[0]);
        return 1;
    }

    const char *arq_pedidos   = argv[1];
    const char *arq_historico = argv[2];

    /* Pedidos: sem esse arquivo nao ha jogo, entao aborta. */
    FILE *fp = fopen(arq_pedidos, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", arq_pedidos);
        return 1;
    }

    /* Historico: pode nao existir na primeira execucao (spec, Apendice C).
     * Nesse caso, segue com historico vazio. */
    FILE *fh = fopen(arq_historico, "r");
    if (fh == NULL) {
        printf("Historico '%s' nao encontrado: iniciando vazio.\n", arq_historico);
    }

    /* TODO (ordem sugerida):
     *  1. carregar pedidos em uma AVL (medir tempo com timer.h)
     *  2. carregar historico em uma lista contigua
     *  3. gravar metricas de carregamento em medicoes.log
     *  4. exibir o menu
     */

    fclose(fp);
    if (fh != NULL) fclose(fh);
    return 0;
}
