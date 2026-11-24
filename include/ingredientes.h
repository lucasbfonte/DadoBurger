/*
 * ingredientes.h - Tabela 1 da especificacao em forma de codigo.

  Centraliza simbolo, nome e categoria de cada ingrediente. Assim o resto
  do programa (heap, pilha, interface, verificacao) nunca compara com
  letras soltas tipo 'n' ou 'b' espalhadas pelo codigo.
 */
#ifndef INGREDIENTES_H
#define INGREDIENTES_H

/* Um valor por ingrediente. ING_TOTAL fica por ultimo e vale a quantidade
 * de ingredientes: serve para declarar vetores do tipo estoque[ING_TOTAL]. */
typedef enum {
    ING_PAO_CIMA,   /* z */
    ING_KETCHUP,    /* k */
    ING_MOSTARDA,   /* m */
    ING_PICLES,     /* p */
    ING_ALFACE,     /* a */
    ING_QUEIJO_SUICO,   /* s */
    ING_CHEDDAR,    /* q */
    ING_CEBOLA,     /* c */
    ING_TOMATE,     /* t */
    ING_CARNE,      /* n */
    ING_OVO,        /* o */
    ING_BACON,      /* b */
    ING_TOTAL
} Ingrediente;

typedef enum {
    CAT_SEM_PREPARO,  /* quantidade infinita, nao precisa preparar */
    CAT_PERENE,       /* precisa preparar, nao vence */
    CAT_PERECIVEL     /* precisa preparar e vence (vai para um heap) */
} Categoria;

#define RECEITA_MIN 5
#define RECEITA_MAX 7

/* Converte simbolo ('n') em ingrediente (ING_CARNE). Retorna -1 se invalido. */
int         ingrediente_de_simbolo(char simbolo);

char        ingrediente_simbolo(Ingrediente ing);
const char *ingrediente_nome(Ingrediente ing);
Categoria   ingrediente_categoria(Ingrediente ing);

/* 1 se a string tem de 5 a 7 simbolos validos, 0 caso contrario. */
int         receita_valida(const char *receita);

#endif
