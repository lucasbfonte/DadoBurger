/* textos.h
 * Textos mostrados ao jogador, compartilhados pelas duas interfaces
 * (assim "Como jogar" e as mensagens de acao ficam iguais nas duas).
 */
#ifndef TEXTOS_H
#define TEXTOS_H

#include "partida.h"

/* Mensagem curta para o resultado de uma acao. */
const char *textoResultado(ResultadoAcao r);

/* Linhas da tela "Como jogar", terminadas por NULL. Linhas que comecam
 * com '#' sao titulos. */
extern const char *TEXTO_COMO_JOGAR[];

#endif
