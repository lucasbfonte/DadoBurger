/* pedidos.c
 * AVL + cache hash dos pedidos. Ver pedidos.h.
 */
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"
#include "pedidos.h"
#include "timer.h"

int pedidosIniciar(Pedidos *p, int m)
{
    avlIniciar(&p->avl);
    p->tempo_carga = 0.0;
    p->duplicatas = 0;
    p->tempo_buscas_avl = 0.0;
    return encInicializar(&p->cache, m);
}

void pedidosDestruir(Pedidos *p)
{
    avlDestruir(&p->avl);
    encDestruir(&p->cache);
}

ResultadoCarga pedidosCarregar(Pedidos *p, FILE *fp, long *linha_erro)
{
    int  codigo;
    char receita[RECEITA_MAX + 1];
    ResultadoLeitura r;
    ResultadoCarga res = CARGA_OK;

    Timer t;
    timer_iniciar(&t);

    while ((r = ler_pedido(fp, &codigo, receita, linha_erro)) == LEITURA_OK) {
        int st = avlInserir(&p->avl, codigo, receita);
        if (st < 0) { res = CARGA_ERRO_MEMORIA; break; }
        if (st == 0) p->duplicatas++;
    }

    p->tempo_carga = timer_parar(&t);

    if (res != CARGA_OK)         return res;
    if (r == LEITURA_ERRO)       return CARGA_ERRO_FORMATO;
    if (p->avl.nos == 0)         return CARGA_VAZIO;
    return CARGA_OK;
}

/* Inteiro aleatorio em [min, max].
 * (unsigned long long porque no Windows o long tem so 32 bits.)
 * No Windows RAND_MAX vale so 32767, menor que o numero de pedidos
 * (837 mil): com um rand() so, codigos acima de 32767 nunca sairiam.
 * Por isso combina dois rand() para ter ~30 bits. */
static int sortear_entre(int min, int max)
{
    unsigned long long faixa = (unsigned long long)((long long)max - min) + 1;
    unsigned long long r = (unsigned long long)rand() * ((unsigned long long)RAND_MAX + 1)
                         + (unsigned long long)rand();
    return (int)(min + (long long)(r % faixa));
}

int pedidosSortear(Pedidos *p, int *codigo, char receita[RECEITA_MAX + 1])
{
    int c = sortear_entre(avlMinimo(&p->avl), avlMaximo(&p->avl));

    /* 1. Cache */
    const char *rec = encBuscar(&p->cache, c);
    if (rec) {
        *codigo = c;
        strcpy(receita, rec);
        return 1;
    }

    /* 2. Miss: AVL, com o tempo acumulado em silencio (secao 5.1).
     * Busca o "teto": se c existe, e ele mesmo; se c caiu num buraco do
     * intervalo, e o proximo codigo existente. Nunca e NULL, porque
     * c <= maximo da arvore. Assim cada cliente custa exatamente uma
     * busca, e as metricas nao ficam poluidas por buscas fracassadas. */
    Timer t;
    timer_iniciar(&t);
    const NodoAvl *n = avlBuscarTeto(&p->avl, c);
    p->tempo_buscas_avl += timer_parar(&t);

    /* Guarda no cache com o codigo real do pedido encontrado. Se c caiu
     * num buraco, o pedido encontrado pode ja estar no cache (veio de
     * outro sorteio): nesse caso nao insere de novo. */
    if (!encContem(&p->cache, n->codigo)) {
        if (!encInserir(&p->cache, n->codigo, n->receita))
            return 0;
    }
    *codigo = n->codigo;
    strcpy(receita, n->receita);
    return 1;
}
