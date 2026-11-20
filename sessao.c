/* sessao.c
 * Ver sessao.h.
 */
#include <stdio.h>

#include "config.h"
#include "relatorio.h"
#include "sessao.h"

int sessaoIniciar(Sessao *s, const char *arq_pedidos,
                  const char *arq_historico, char *erro, int tam_erro)
{
    long linha = 0;

    s->log = NULL;
    s->arq_historico = arq_historico;
    s->historico_existia = 0;

    if (!pedidosIniciar(&s->pedidos, HASH_M)) {
        snprintf(erro, tam_erro, "Sem memoria para a tabela hash.");
        return 0;
    }
    if (!historicoIniciar(&s->historico)) {
        pedidosDestruir(&s->pedidos);
        snprintf(erro, tam_erro, "Sem memoria para o historico.");
        return 0;
    }

    /* 1. Pedidos -> AVL (sem esse arquivo nao ha jogo) */
    FILE *fp = fopen(arq_pedidos, "r");
    if (fp == NULL) {
        snprintf(erro, tam_erro, "Nao foi possivel abrir '%s'.", arq_pedidos);
        goto falha;
    }
    ResultadoCarga rc = pedidosCarregar(&s->pedidos, fp, &linha);
    fclose(fp);
    if (rc == CARGA_ERRO_FORMATO) {
        snprintf(erro, tam_erro, "'%s': formato invalido na linha %ld.",
                 arq_pedidos, linha);
        goto falha;
    }
    if (rc == CARGA_ERRO_MEMORIA) {
        snprintf(erro, tam_erro, "Sem memoria carregando os pedidos.");
        goto falha;
    }
    if (rc == CARGA_VAZIO) {
        snprintf(erro, tam_erro, "'%s' nao tem nenhum pedido.", arq_pedidos);
        goto falha;
    }

    /* 2. Historico -> lista contigua (pode nao existir: comeca vazio) */
    FILE *fh = fopen(arq_historico, "r");
    s->historico_existia = (fh != NULL);
    int rh = historicoCarregar(&s->historico, fh, &linha);
    if (fh) fclose(fh);
    if (rh == 0) {
        snprintf(erro, tam_erro, "'%s': formato invalido na linha %ld.",
                 arq_historico, linha);
        goto falha;
    }
    if (rh < 0) {
        snprintf(erro, tam_erro, "Sem memoria carregando o historico.");
        goto falha;
    }

    s->historico_carregado = s->historico.n;

    /* 3. Log (criado/sobrescrito a cada execucao) + metricas 5.1 */
    s->log = fopen(ARQ_LOG, "w");
    if (s->log == NULL) {
        snprintf(erro, tam_erro, "Nao foi possivel criar '%s'.", ARQ_LOG);
        goto falha;
    }
    relatorioCabecalho(s->log);
    relatorioCargaAvl(s->log, &s->pedidos);
    relatorioCargaHistorico(s->log, s->historico_carregado);
    return 1;

falha:
    historicoDestruir(&s->historico);
    pedidosDestruir(&s->pedidos);
    return 0;
}

void sessaoEncerrar(Sessao *s)
{
    if (s->log) fclose(s->log);
    s->log = NULL;
    historicoDestruir(&s->historico);
    pedidosDestruir(&s->pedidos);
}

int sessaoNovaPartida(Sessao *s, Partida *p)
{
    return partidaIniciar(p, &s->pedidos, N_CLIENTES, PACIENCIA_MAX);
}

void sessaoRelatorio(Sessao *s, FILE *out)
{
    relatorioCabecalho(out);
    relatorioCargaAvl(out, &s->pedidos);
    relatorioCargaHistorico(out, s->historico_carregado);
    relatorioBuscasAvl(out, &s->pedidos);
    relatorioHash(out, &s->pedidos);
    relatorioRodape(out);
}

int sessaoFimDePartida(Sessao *s, const char *nome, int pontos)
{
    int ok = historicoInserirFim(&s->historico, nome, pontos);
    if (ok)
        ok = gravar_historico(s->arq_historico, nome, pontos);
    s->historico_existia = s->historico_existia || ok;

    fprintf(s->log, ">> Fim de partida: %s, %d ponto(s)\n\n", nome, pontos);
    sessaoRelatorio(s, s->log);
    return ok;
}

double sessaoOrdenarHistorico(Sessao *s, Historico *copia, CriterioOrdem c)
{
    /* A copia fica fora do intervalo cronometrado (secao 5.2). */
    if (!historicoCopiar(copia, &s->historico))
        return -1.0;

    double t = historicoOrdenar(copia, c);
    if (t >= 0)
        relatorioOrdenacao(s->log, copia->n, c, t);
    return t;
}
