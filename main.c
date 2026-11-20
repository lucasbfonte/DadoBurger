/*
 * Dado Burger - INF01203 (UFRGS)
 * Ponto de entrada: le os nomes dos arquivos via argv, carrega tudo
 * (antes de exibir o menu, secoes 4.1 e 4.2) e entrega para a interface.
 *
 * Uso: ./dado_burger <pedidos> <historico>
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "interface.h"
#include "sessao.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <pedidos> <historico>\n", argv[0]);
        return 1;
    }

    srand((unsigned) time(NULL));   /* pedidosSortear usa rand() */

    Sessao s;
    char erro[256];

    printf("Carregando pedidos de '%s'...\n", argv[1]);
    if (!sessaoIniciar(&s, argv[1], argv[2], erro, sizeof erro)) {
        fprintf(stderr, "Erro: %s\n", erro);
        return 1;
    }
    if (!s.historico_existia)
        printf("Historico '%s' nao encontrado: iniciando vazio.\n", argv[2]);
    printf("%ld pedidos na AVL, %d entradas no historico.\n",
           s.pedidos.avl.nos, s.historico.n);

    int ret = interfaceExecutar(&s);

    sessaoEncerrar(&s);
    return ret;
}
