/* personagens.c
 * Tabela de clientes. Ver personagens.h.
 */
#include <stdlib.h>

#include "personagens.h"

/*  nome              pele        cabelo      roupa       estilo          barba            gris oc  fem */
static const Personagem TABELA[] = {
    { "Dennis",       0xF1C9A5FF, 0x1E1A18FF, 0x3D5A99FF, CABELO_CURTO,  BARBA_POR_FAZER, 0,   0,  0 },
    { "Marco A.",     0xEBC19BFF, 0x4A4643FF, 0x4E8C5AFF, CABELO_CURTO,  BARBA_NENHUMA,   1,   0,  0 },
    { "Luciano",      0xE3B48CFF, 0x1B1716FF, 0x8C2F39FF, CABELO_CURTO,  BARBA_NENHUMA,   0,   0,  0 },
    { "Reitora Marcia", 0xEFC3A0FF, 0x161212FF, 0x2C3E66FF, CABELO_LONGO, BARBA_NENHUMA,  0,   0,  1 },
};

#define TOTAL ((int) (sizeof TABELA / sizeof TABELA[0]))

static int deslocamento = 0;

int personagensTotal(void) {
    return TOTAL;
}

void personagensSortear(void) {
    deslocamento = rand() % TOTAL;
}

const Personagem *personagemDoCliente(int id) {
    return &TABELA[(deslocamento + id - 1) % TOTAL];
}
