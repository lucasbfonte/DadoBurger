/* partida.c
 * Regras da partida. Ver partida.h para o ciclo de um turno.
 */
#include <string.h>

#include "config.h"
#include "partida.h"

static const Ingrediente PERECIVEIS[N_PERECIVEIS] = {
    ING_CARNE, ING_OVO, ING_BACON
};

/* ------------------------------------------------------------------ */
/*  Consultas simples                                                   */
/* ------------------------------------------------------------------ */

int partidaIndicePerecivel(Ingrediente ing) {
    for (int i = 0; i < N_PERECIVEIS; i++)
        if (PERECIVEIS[i] == ing)
            return i;
    return -1;
}

Ingrediente partidaPerecivelDoIndice(int i) {
    return PERECIVEIS[i];
}

int partidaValidade(Ingrediente ing) {
    switch (ing) {
        case ING_CARNE: return VALIDADE_CARNE;
        case ING_OVO:   return VALIDADE_OVO;
        case ING_BACON: return VALIDADE_BACON;
        default:        return 0;
    }
}

int partidaEmAndamento(const Partida *p) {
    return p->estado == PARTIDA_JOGANDO;
}

int partidaPontuacao(const Partida *p) {
    return p->atendidos;
}

Cliente *partidaClienteAtual(Partida *p) {
    return partidaEmAndamento(p) ? filaFrente(&p->fila) : NULL;
}

int partidaQuantidade(const Partida *p, Ingrediente ing) {
    switch (ingrediente_categoria(ing)) {
        case CAT_SEM_PREPARO: return -1;
        case CAT_PERENE:      return p->estoque[ing];
        case CAT_PERECIVEL:
        default:
            return heapTamanho(&p->pereciveis[partidaIndicePerecivel(ing)]);
    }
}

int partidaProximoVencimento(const Partida *p, Ingrediente ing, int *turno) {
    int i = partidaIndicePerecivel(ing);
    if (i < 0)
        return 0;
    return heapTopo(&p->pereciveis[i], turno);
}

/* ------------------------------------------------------------------ */
/*  Internos                                                            */
/* ------------------------------------------------------------------ */

/* Inicio de turno (secao 4.7): enquanto o topo de cada heap vencer
 * neste turno ou antes, descarta. */
static void descartarVencidos(Partida *p) {
    for (int i = 0; i < N_PERECIVEIS; i++) {
        int venc;
        p->vencidos_no_turno[i] = 0;
        while (heapTopo(&p->pereciveis[i], &venc) && venc <= p->turno) {
            heapRemover(&p->pereciveis[i], &venc);
            p->vencidos_no_turno[i]++;
        }
    }
}

/* Sorteia o pedido do cliente que acabou de chegar na frente e monta a
 * lista encadeada da receita dele. */
static int prepararClienteDaFrente(Partida *p) {
    Cliente *c = filaFrente(&p->fila);
    if (c == NULL)
        return 1;
    if (!pedidosSortear(p->pedidos, &c->codigo, c->receita))
        return 0;
    listaDestruir(&p->receita);
    return listaMontar(&p->receita, c->receita);
}

/* Cliente da frente atendido: sai da fila e o proximo assume. */
static int atenderCliente(Partida *p) {
    Cliente atendido;
    filaDesenfileirar(&p->fila, &atendido);
    p->atendidos++;
    pilhaEsvaziar(&p->hamburguer);        /* prato novo */

    if (filaVazia(&p->fila)) {
        p->estado = PARTIDA_VITORIA;
        listaDestruir(&p->receita);
        return 1;
    }
    return prepararClienteDaFrente(p);
}

/* Fecha um turno que foi consumido. 'atendeu' = a acao concluiu o pedido
 * (ai o cliente ja saiu e a paciencia nao conta). */
static void fecharTurno(Partida *p, int atendeu) {
    if (!atendeu && p->estado == PARTIDA_JOGANDO) {
        Cliente *c = filaFrente(&p->fila);
        c->paciencia--;
        if (c->paciencia <= 0) {
            p->estado = PARTIDA_DERROTA;
            return;
        }
    }
    p->turno++;
    descartarVencidos(p);
}

/* ------------------------------------------------------------------ */
/*  Ciclo de vida                                                       */
/* ------------------------------------------------------------------ */

int partidaIniciar(Partida *p, Pedidos *pedidos, int n_clientes, int paciencia) {
    memset(p, 0, sizeof(*p));
    p->pedidos = pedidos;
    p->paciencia_max = paciencia;
    p->total_clientes = n_clientes;
    p->turno = 1;
    p->estado = PARTIDA_JOGANDO;

    filaInicializar(&p->fila);
    listaInicializar(&p->receita);
    pilhaInicializa(&p->hamburguer);

    for (int i = 0; i < N_PERECIVEIS; i++)
        if (!heapIniciar(&p->pereciveis[i], 8)) {
            partidaDestruir(p);
            return 0;
        }

    /* Todos os clientes entram na fila no inicio, em ordem de chegada. */
    for (int i = 1; i <= n_clientes; i++) {
        Cliente c;
        c.id = i;
        c.paciencia = paciencia;
        c.codigo = 0;
        c.receita[0] = '\0';
        if (!filaEnfileirar(&p->fila, c)) {
            partidaDestruir(p);
            return 0;
        }
    }

    if (!prepararClienteDaFrente(p)) {
        partidaDestruir(p);
        return 0;
    }
    descartarVencidos(p);   /* inicio do turno 1 (nada vence ainda) */
    return 1;
}

void partidaDestruir(Partida *p) {
    filaDestruir(&p->fila);
    listaDestruir(&p->receita);
    pilhaEsvaziar(&p->hamburguer);
    for (int i = 0; i < N_PERECIVEIS; i++)
        heapDestruir(&p->pereciveis[i]);
}

void partidaDesistir(Partida *p) {
    if (partidaEmAndamento(p))
        p->estado = PARTIDA_DESISTIU;
}

/* ------------------------------------------------------------------ */
/*  Acoes                                                               */
/* ------------------------------------------------------------------ */

ResultadoAcao partidaPreparar(Partida *p, Ingrediente ing) {
    if (!partidaEmAndamento(p))
        return ACAO_PARTIDA_ENCERRADA;

    switch (ingrediente_categoria(ing)) {
        case CAT_SEM_PREPARO:
            return ACAO_NAO_PRECISA_PREPARO;

        case CAT_PERENE:
            p->estoque[ing]++;
            break;

        case CAT_PERECIVEL: {
            int i = partidaIndicePerecivel(ing);
            /* vencimento = turno de preparo + validade */
            if (!heapInserir(&p->pereciveis[i], p->turno + partidaValidade(ing)))
                return ACAO_ERRO_MEMORIA;
            break;
        }
    }
    fecharTurno(p, 0);
    return ACAO_OK;
}

ResultadoAcao partidaAdicionar(Partida *p, Ingrediente ing) {
    Ingrediente topo;

    if (!partidaEmAndamento(p))
        return ACAO_PARTIDA_ENCERRADA;

    /* Nada entra por cima do pao de cima. */
    if (pilhaTopo(&p->hamburguer, &topo) && topo == ING_PAO_CIMA)
        return ACAO_HAMBURGUER_FECHADO;

    /* Tira do estoque (sem preparo e infinito). */
    switch (ingrediente_categoria(ing)) {
        case CAT_SEM_PREPARO:
            break;
        case CAT_PERENE:
            if (p->estoque[ing] == 0)
                return ACAO_SEM_ESTOQUE;
            p->estoque[ing]--;
            break;
        case CAT_PERECIVEL: {
            int venc;   /* sai a unidade que vence primeiro (topo) */
            if (!heapRemover(&p->pereciveis[partidaIndicePerecivel(ing)], &venc))
                return ACAO_SEM_ESTOQUE;
            break;
        }
    }

    if (!pilhaEmpilha(&p->hamburguer, ing))
        return ACAO_ERRO_MEMORIA;

    /* Fechou com o pao de cima: confere com a receita. */
    if (ing == ING_PAO_CIMA) {
        int r = pilhaVerifica(&p->hamburguer, &p->receita);
        if (r < 0)
            return ACAO_ERRO_MEMORIA;
        if (r == 1) {
            if (!atenderCliente(p))
                return ACAO_ERRO_MEMORIA;
            fecharTurno(p, 1);
            return ACAO_ATENDEU;
        }
        fecharTurno(p, 0);
        return ACAO_RECEITA_ERRADA;
    }

    fecharTurno(p, 0);
    return ACAO_OK;
}

ResultadoAcao partidaRemover(Partida *p) {
    Ingrediente lixo;

    if (!partidaEmAndamento(p))
        return ACAO_PARTIDA_ENCERRADA;

    /* O ingrediente removido e descartado: nao volta ao estoque. */
    if (!pilhaDesempilha(&p->hamburguer, &lixo))
        return ACAO_HAMBURGUER_VAZIO;

    fecharTurno(p, 0);
    return ACAO_OK;
}
