/* personagens.h
 * Quem sao os clientes do Antonio's. So aparencia e nome: a fila e a
 * paciencia continuam em partida.c. Para adicionar alguem, basta
 * acrescentar uma linha na tabela de personagens.c.
 *
 * Cores em 0xRRGGBBAA (sem depender da raylib, para a interface de
 * texto tambem poder mostrar os nomes).
 */
#ifndef PERSONAGENS_H
#define PERSONAGENS_H

typedef enum {
    CABELO_CURTO,
    CABELO_LONGO,
    CABELO_CARECA,
    CABELO_CACHEADO,
    CABELO_COQUE
} EstiloCabelo;

typedef enum {
    BARBA_NENHUMA,
    BARBA_POR_FAZER,
    BARBA_CHEIA
} Barba;

typedef struct {
    const char  *nome;
    unsigned int pele;
    unsigned int cabelo;
    unsigned int roupa;
    EstiloCabelo estilo;
    Barba        barba;
    int          grisalho;   /* mechas brancas no cabelo */
    int          oculos;
    int          feminino;   /* cilios e batom */
} Personagem;

int               personagensTotal(void);

/* Sorteia por qual personagem a fila comeca (chamar a cada partida). */
void              personagensSortear(void);

/* Personagem do cliente de numero 'id' (1, 2, 3...). Com menos
 * personagens que clientes, eles se repetem em ordem. */
const Personagem *personagemDoCliente(int id);

#endif
