/* textos.c
 * Ver textos.h. Os numeros vem do config.h, entao a tela "Como jogar"
 * acompanha qualquer ajuste de parametro.
 */
#include "config.h"
#include "textos.h"

#define STR_(x) #x
#define STR(x)  STR_(x)

const char *textoResultado(ResultadoAcao r)
{
    switch (r) {
        case ACAO_OK:                  return "Ok.";
        case ACAO_ATENDEU:             return "Pedido certo! Cliente atendido.";
        case ACAO_RECEITA_ERRADA:      return "Hambúrguer não bate com o pedido. Remova e conserte.";
        case ACAO_SEM_ESTOQUE:         return "Sem estoque: prepare esse ingrediente antes.";
        case ACAO_NAO_PRECISA_PREPARO: return "Esse ingrediente não precisa de preparo.";
        case ACAO_HAMBURGUER_FECHADO:  return "O pão de cima já fechou o hambúrguer.";
        case ACAO_HAMBURGUER_VAZIO:    return "Não há nada no prato para remover.";
        case ACAO_PARTIDA_ENCERRADA:   return "A partida já terminou.";
        case ACAO_ERRO_MEMORIA:        return "Erro: falta de memória.";
    }
    return "";
}

const char *TEXTO_COMO_JOGAR[] = {
    "#Objetivo",
    "Você é o novo funcionário do Dado Burger. Monte o hambúrguer de cada",
    "cliente exatamente como no pedido, de baixo para cima. O pão de baixo",
    "já está no prato; o pedido termina com o pão de cima (z).",
    "",
    "#Ingredientes",
    "Sem preparo (infinitos):  z pão de cima   k ketchup   m mostarda   p picles   a alface",
    "Precisam de preparo:      s queijo suíço  q cheddar   c cebola     t tomate",
    "Perecíveis (vencem):      n carne (" STR(VALIDADE_CARNE) " turnos)   o ovo frito ("
        STR(VALIDADE_OVO) ")   b bacon (" STR(VALIDADE_BACON) ")",
    "",
    "#Ações (cada uma gasta 1 turno)",
    "Preparar um ingrediente  -> +1 no estoque",
    "Adicionar um ingrediente -> empilha no hambúrguer",
    "Remover o topo           -> o ingrediente vai para o lixo",
    "Olhar pedido e estoque não gasta turno.",
    "",
    "#Vitória e derrota",
    "São " STR(N_CLIENTES) " clientes na fila. O da frente perde 1 de paciência a cada turno",
    "(começa com " STR(PACIENCIA_MAX) "). Se chegar a zero, fim de jogo. Atenda todos para vencer.",
    "Pontuação = clientes atendidos.",
    NULL
};
