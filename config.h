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

/* TODO: PACIENCIA_MAX, N_CLIENTES, VALIDADE_CARNE/OVO/BACON */

#endif /* CONFIG_H */
