/* pedidos.h
 * Junta a AVL (armazenamento) e a hash (cache) dos pedidos.
 *
 * Fluxo de uma busca (secao 4.3 do enunciado):
 *   sorteia codigo -> consulta a hash
 *     hit:  devolve a receita sem tocar na AVL
 *     miss: busca na AVL (cronometrado) e insere na hash
 *
 * Este modulo nao imprime nada: devolve codigos de erro e quem chama
 * (a interface) decide o que mostrar.
 */
#ifndef PEDIDOS_H
#define PEDIDOS_H

#include <stdio.h>

#include "avl.h"
#include "hash.h"
#include "ingredientes.h"

typedef struct {
    Avl       avl;
    TabelaEnc cache;

    double tempo_carga;        /* leitura do arquivo + insercoes na AVL */
    long   duplicatas;         /* codigos repetidos ignorados na carga  */
    double tempo_buscas_avl;   /* soma dos tempos das buscas na AVL     */
} Pedidos;

typedef enum {
    CARGA_OK,
    CARGA_ERRO_FORMATO,   /* *linha_erro recebe a linha */
    CARGA_ERRO_MEMORIA,
    CARGA_VAZIO           /* arquivo sem nenhum pedido */
} ResultadoCarga;

/* Cria a AVL vazia e a hash com m slots. Retorna 0 se faltou memoria. */
int  pedidosIniciar(Pedidos *p, int m);
void pedidosDestruir(Pedidos *p);

/* Le todos os pedidos de fp para a AVL, medindo o tempo. */
ResultadoCarga pedidosCarregar(Pedidos *p, FILE *fp, long *linha_erro);

/* Sorteia um codigo existente: grava em *codigo e copia a receita.
 * Se o codigo sorteado nao existir (arquivo com buracos), usa o proximo
 * codigo existente (busca "teto" na AVL).
 * Retorna 1, ou 0 se faltou memoria ao inserir no cache.
 * (O codigo vai por parametro porque o arquivo final pode ter codigos
 * negativos, entao nao da para usar -1 como sinal de erro.)
 * Pre-condicao: srand() ja chamado e AVL nao vazia. */
int  pedidosSortear(Pedidos *p, int *codigo, char receita[RECEITA_MAX + 1]);

#endif /* PEDIDOS_H */
