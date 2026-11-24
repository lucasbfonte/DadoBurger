/* partida.h
 * Regras de uma partida do Dado Burger (secoes 3.3, 3.4 e 4.7).
 *
 * Junta as estruturas: fila de clientes, lista da receita do cliente da
 * frente, pilha do hamburguer, estoque dos perenes (contadores) e um
 * heap minimo por perecivel.
 *
 * Este modulo nao imprime nem le teclado (regra do README): cada acao
 * devolve um ResultadoAcao e a interface decide o que mostrar. Por isso
 * a mesma logica serve para a versao texto e para a versao raylib.
 *
 * Ciclo de um turno:
 *   1. inicio do turno: descarta pereciveis vencidos (vencimento <= turno)
 *   2. o jogador faz UMA acao que consome turno (preparar, adicionar,
 *      remover). Acoes invalidas nao consomem turno.
 *   3. se a acao concluiu o pedido: cliente sai da fila, proximo vem
 *      para a frente (e sorteia o pedido dele)
 *      senao: paciencia do cliente da frente - 1; se chegou a 0, derrota
 *   4. turno + 1 e volta ao passo 1
 */
#ifndef PARTIDA_H
#define PARTIDA_H

#include "fila.h"
#include "heap.h"
#include "ingredientes.h"
#include "lista.h"
#include "pedidos.h"
#include "pilha.h"

#define N_PERECIVEIS 3   /* carne, ovo, bacon: um heap para cada */

typedef enum {
    PARTIDA_JOGANDO,
    PARTIDA_VITORIA,
    PARTIDA_DERROTA,
    PARTIDA_DESISTIU
} EstadoPartida;

typedef enum {
    ACAO_OK,                 /* consumiu turno */
    ACAO_ATENDEU,            /* consumiu turno e o cliente foi atendido */
    ACAO_RECEITA_ERRADA,     /* fechou com o pao mas nao bate (consumiu turno) */
    ACAO_SEM_ESTOQUE,        /* ingrediente nao preparado: nao consome */
    ACAO_NAO_PRECISA_PREPARO,/* tentou preparar ketchup etc.: nao consome */
    ACAO_HAMBURGUER_FECHADO, /* tentou adicionar por cima do pao: nao consome */
    ACAO_HAMBURGUER_VAZIO,   /* tentou remover do prato vazio: nao consome */
    ACAO_PARTIDA_ENCERRADA,  /* partida ja acabou: nao consome */
    ACAO_ERRO_MEMORIA
} ResultadoAcao;

typedef struct {
    Pedidos *pedidos;            /* AVL + cache (nao e dono) */

    FilaEnc  fila;               /* clientes esperando; frente = atendido */
    ListaEnc receita;            /* receita do cliente da frente */
    PilhaEnc hamburguer;         /* o que esta no prato (sem o pao de baixo) */

    int      estoque[ING_TOTAL];         /* so usado pelos perenes */
    HeapMin  pereciveis[N_PERECIVEIS];   /* turnos de vencimento */

    int      turno;              /* turno atual, comeca em 1 */
    int      paciencia_max;
    int      total_clientes;
    int      atendidos;          /* = pontuacao */
    EstadoPartida estado;

    /* Para a interface avisar o jogador: quantas unidades de cada
     * perecivel venceram no inicio do turno atual. */
    int      vencidos_no_turno[N_PERECIVEIS];
} Partida;

/* Enfileira os clientes e sorteia o pedido do primeiro.
 * Retorna 0 se faltou memoria (nesse caso ja liberou o que alocou). */
int  partidaIniciar(Partida *p, Pedidos *pedidos, int n_clientes, int paciencia);
void partidaDestruir(Partida *p);

/* Acoes que consomem turno (secao 3.4). */
ResultadoAcao partidaPreparar(Partida *p, Ingrediente ing);
ResultadoAcao partidaAdicionar(Partida *p, Ingrediente ing);
ResultadoAcao partidaRemover(Partida *p);

/* Saida voluntaria: encerra com a pontuacao atual. */
void partidaDesistir(Partida *p);

/* Consultas (nao consomem turno) */
int      partidaEmAndamento(const Partida *p);
int      partidaPontuacao(const Partida *p);
Cliente *partidaClienteAtual(Partida *p);               /* NULL se acabou */

/* Quantidade disponivel: -1 = infinito (sem preparo); perene: contador;
 * perecivel: tamanho do heap. */
int      partidaQuantidade(const Partida *p, Ingrediente ing);

/* Turno em que vence a proxima unidade do perecivel. 0 se nao ha. */
int      partidaProximoVencimento(const Partida *p, Ingrediente ing, int *turno);

/* Validade configurada (turnos) de um perecivel; 0 se nao for perecivel. */
int      partidaValidade(Ingrediente ing);

/* Indice 0..N_PERECIVEIS-1 do heap de um perecivel, ou -1. */
int      partidaIndicePerecivel(Ingrediente ing);
Ingrediente partidaPerecivelDoIndice(int i);

#endif /* PARTIDA_H */
