/* config.h
 * Parametros configuraveis do jogo (secao 3.2 do enunciado).
 * Cada valor precisa ser justificado no relatorio: ver README.md.
 */
#ifndef CONFIG_H
#define CONFIG_H

/* Tamanho da tabela hash (cache de pedidos).
 * Primo, longe de potencias de 2 e de 10 (recomendacao da aula para o
 * hash por divisao). Dimensionado para o numero esperado de pedidos
 * buscados numa sessao: algumas partidas com ~10-20 clientes cada
 * dao da ordem de 100 entradas, o que mantem lambda <= ~0.5. */
#define HASH_M 211

/* Paciencia maxima de cada cliente, em turnos.
 * No pedidos.txt, o minimo de turnos para montar uma receita (1 por
 * ingrediente + 1 de preparo para cada um que precisa) vai de 6 a 13,
 * com media ~11. Com 16, ate a receita mais longa sobra folga de 3
 * turnos para erros, mas preparar demais "para o proximo" pesa. */
#define PACIENCIA_MAX 16

/* Clientes por partida: ~10 pedidos de ~11 turnos = ~110 acoes,
 * uma partida de poucos minutos. */
#define N_CLIENTES 10

/* Validade dos pereciveis, em turnos depois do preparo.
 * Ovo vence rapido (exige preparar na hora); bacon dura mais (da para
 * adiantar para o proximo cliente). Carne fica no meio. */
#define VALIDADE_CARNE  8
#define VALIDADE_OVO    5
#define VALIDADE_BACON 10

#endif /* CONFIG_H */
