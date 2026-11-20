/* interface.h
 * Unico modulo que conversa com o usuario (regra do README).
 * Ha duas implementacoes com a mesma funcao:
 *   interface_raylib.c  -> janela grafica (extra da secao 8)
 *   interface_texto.c   -> terminal (compila so com o comando do Apendice C)
 * O Makefile escolhe qual entra no executavel.
 */
#ifndef INTERFACE_H
#define INTERFACE_H

#include "sessao.h"

/* Roda o menu ate o usuario sair. Retorna o codigo de saida do programa. */
int interfaceExecutar(Sessao *s);

#endif
