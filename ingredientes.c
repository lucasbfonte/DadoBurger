
#include <string.h>

#include "Ingredientes.h"

typedef struct {
    char        simbolo;
    const char *nome;
    Categoria   categoria;
} InfoIngrediente;

/* A ordem aqui TEM que bater com a ordem do enum em ingredientes.h:
 * TABELA[ING_CARNE] precisa ser a linha da carne. */
static const InfoIngrediente TABELA[ING_TOTAL] = {
    { 'z', "Pao (de cima)",  CAT_SEM_PREPARO },
    { 'k', "Ketchup",        CAT_SEM_PREPARO },
    { 'm', "Mostarda",       CAT_SEM_PREPARO },
    { 'p', "Picles",         CAT_SEM_PREPARO },
    { 'a', "Alface",         CAT_SEM_PREPARO },
    { 's', "Queijo suico",   CAT_PERENE      },
    { 'q', "Queijo cheddar", CAT_PERENE      },
    { 'c', "Cebola",         CAT_PERENE      },
    { 't', "Tomate",         CAT_PERENE      },
    { 'n', "Carne",          CAT_PERECIVEL   },
    { 'o', "Ovo frito",      CAT_PERECIVEL   },
    { 'b', "Bacon",          CAT_PERECIVEL   },
};

int ingrediente_de_simbolo(char simbolo)
{
    for (int i = 0; i < ING_TOTAL; i++)
        if (TABELA[i].simbolo == simbolo)
            return i;
    return -1;
}

char ingrediente_simbolo(Ingrediente ing) {
    return TABELA[ing].simbolo;
}
const char *ingrediente_nome(Ingrediente ing) {
    return TABELA[ing].nome;
}
Categoria ingrediente_categoria(Ingrediente ing) {
    return TABELA[ing].categoria;
}

int receita_valida(const char *receita) {
    int n = strlen(receita);
    if (n < RECEITA_MIN || n > RECEITA_MAX)
        return 0;
    for (int i = 0; i < n; i++)
        if (ingrediente_de_simbolo(receita[i]) < 0)
            return 0;
    return 1;
}
