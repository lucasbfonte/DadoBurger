/* relatorio.c
 * Ver relatorio.h.
 */
#include "relatorio.h"

void relatorioCabecalho(FILE *out)
{
    fprintf(out, "=== RELATORIO DE DESEMPENHO ===\n\n");
}

void relatorioCargaAvl(FILE *out, const Pedidos *p)
{
    fprintf(out, "-- AVL: carregamento --\n");
    fprintf(out, "%-30s %ld\n",    "Nos inseridos:",         p->avl.nos);
    fprintf(out, "%-30s %d\n",     "Altura resultante:",     avlAltura(&p->avl));
    fprintf(out, "%-30s %ld\n",    "Rotacoes (insercao):",   p->avl.rotacoes);
    fprintf(out, "%-30s %.6f s\n", "Tempo de carregamento:", p->tempo_carga);
    fprintf(out, "\n");
    fflush(out);
}

void relatorioBuscasAvl(FILE *out, const Pedidos *p)
{
    long b = p->avl.buscas;
    /* Convencao do enunciado: sem buscas, media = 0. */
    double media = b ? (double)p->avl.comparacoes / b : 0.0;

    fprintf(out, "-- AVL: buscas durante a partida --\n");
    fprintf(out, "%-30s %ld\n",    "Total de buscas:",              b);
    fprintf(out, "%-30s %ld\n",    "Total de comparacoes:",         p->avl.comparacoes);
    fprintf(out, "%-30s %.2f\n",   "Media de comparacoes por busca:", media);
    fprintf(out, "%-30s %.6f s\n", "Tempo total acumulado:",        p->tempo_buscas_avl);
    fprintf(out, "\n");
    fflush(out);
}

void relatorioHash(FILE *out, const Pedidos *p)
{
    const TabelaEnc *t = &p->cache;
    fprintf(out, "-- Tabela hash --\n");
    fprintf(out, "%-30s %d\n",     "Tamanho (m):",            t->m);
    fprintf(out, "%-30s %d\n",     "Entradas ocupadas (n):",  t->n);
    fprintf(out, "%-30s %.2f\n",   "Fator de carga (lambda):", encFatorCarga(t));
    fprintf(out, "%-30s %ld\n",    "Total de hits:",          t->hits);
    fprintf(out, "%-30s %ld\n",    "Total de misses:",        t->misses);
    fprintf(out, "%-30s %.1f%%\n", "Taxa de acerto:",         encTaxaAcerto(t));
    fprintf(out, "%-30s %ld\n",    "Total de colisoes:",      t->colisoes);
    fprintf(out, "\n");
    fflush(out);
}
