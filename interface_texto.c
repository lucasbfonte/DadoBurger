/* interface_texto.c
 * Interface de terminal. Compila com o comando do Apendice C (sem raylib)
 * e serve de reserva caso a raylib nao esteja instalada na apresentacao.
 */
#include <stdio.h>
#include <string.h>

#include "interface.h"
#include "personagens.h"
#include "textos.h"

/* Le uma linha do teclado sem o '\n'. Retorna 0 em EOF. */
static int lerLinha(char *buf, int tam)
{
    if (fgets(buf, tam, stdin) == NULL)
        return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

static void pausar(void)
{
    char buf[8];
    printf("\n[Enter] para voltar ao menu...");
    lerLinha(buf, sizeof buf);
}

/* ------------------------------------------------------------------ */

static void mostrarComoJogar(void)
{
    printf("\n========== COMO JOGAR ==========\n");
    for (int i = 0; TEXTO_COMO_JOGAR[i] != NULL; i++) {
        const char *l = TEXTO_COMO_JOGAR[i];
        if (l[0] == '#') printf("\n-- %s --\n", l + 1);
        else             printf("%s\n", l);
    }
    pausar();
}

static void imprimirReceita(const char *receita)
{
    for (int i = 0; receita[i]; i++)
        printf("%s%c", i ? " " : "", receita[i]);
}

/* Hamburguer de baixo para cima: a pilha esta de cima para baixo, entao
 * copia os simbolos para um vetor e imprime ao contrario. */
static void imprimirHamburguer(const PilhaEnc *pilha)
{
    char s[64];
    int n = 0;
    for (const nodoPilha *no = pilha->topo; no && n < 63; no = no->prox)
        s[n++] = ingrediente_simbolo(no->dado);
    printf("[pao]");
    while (n > 0)
        printf(" %c", s[--n]);
}

static void mostrarEstado(Partida *p)
{
    Cliente *c = partidaClienteAtual(p);

    printf("\n------------------------------------------------------------\n");
    printf("Turno %d | Cliente %d de %d (%s) | Atendidos: %d\n",
           p->turno, c->id, p->total_clientes, personagemDoCliente(c->id)->nome, p->atendidos);
    printf("Paciencia do cliente: %d/%d turnos restantes\n",
           c->paciencia, p->paciencia_max);
    printf("Pedido #%d:   ", c->codigo);
    imprimirReceita(c->receita);
    printf("\nHamburguer:   ");
    imprimirHamburguer(&p->hamburguer);
    printf("\n\nEstoque:\n");

    for (int i = 0; i < ING_TOTAL; i++) {
        Ingrediente ing = (Ingrediente) i;
        int q = partidaQuantidade(p, ing), venc;
        printf("  %c %-15s ", ingrediente_simbolo(ing), ingrediente_nome(ing));
        if (q < 0)
            printf("infinito");
        else
            printf("%d", q);
        if (partidaProximoVencimento(p, ing, &venc))
            printf("  (proxima vence em %d turno(s))", venc - p->turno);
        printf("\n");
    }

    for (int i = 0; i < N_PERECIVEIS; i++)
        if (p->vencidos_no_turno[i])
            printf("  ! %d %s venceu(ram) e foi(ram) descartado(s)\n",
                   p->vencidos_no_turno[i],
                   ingrediente_nome(partidaPerecivelDoIndice(i)));
}

/* Le o simbolo depois do comando ("p n" -> 'n'). -1 se invalido. */
static int simboloDoComando(const char *cmd)
{
    const char *s = cmd + 1;
    while (*s == ' ') s++;
    if (s[0] == '\0' || s[1] != '\0')
        return -1;
    return ingrediente_de_simbolo(s[0]);
}

static void novoJogo(Sessao *s)
{
    char nome[NOME_MAX + 1];
    char cmd[64];
    Partida p;

    printf("\nNome do jogador: ");
    if (!lerLinha(cmd, sizeof cmd)) return;
    strncpy(nome, cmd[0] ? cmd : "Anonimo", NOME_MAX);
    nome[NOME_MAX] = '\0';

    personagensSortear();
    if (!sessaoNovaPartida(s, &p)) {
        printf("Erro: falta de memoria para iniciar a partida.\n");
        return;
    }

    printf("\nComandos: p <simbolo> = preparar   a <simbolo> = adicionar\n"
           "          r = remover topo         s = sair da partida\n");

    while (partidaEmAndamento(&p)) {
        mostrarEstado(&p);
        printf("> ");
        if (!lerLinha(cmd, sizeof cmd)) { partidaDesistir(&p); break; }

        ResultadoAcao r;
        int ing;
        switch (cmd[0]) {
            case 'p': case 'a':
                ing = simboloDoComando(cmd);
                if (ing < 0) { printf("Simbolo invalido.\n"); continue; }
                r = cmd[0] == 'p' ? partidaPreparar(&p, (Ingrediente) ing)
                                  : partidaAdicionar(&p, (Ingrediente) ing);
                break;
            case 'r':
                r = partidaRemover(&p);
                break;
            case 's':
                partidaDesistir(&p);
                continue;
            default:
                printf("Comando invalido.\n");
                continue;
        }
        if (r != ACAO_OK)
            printf(">> %s\n", textoResultado(r));
    }

    switch (p.estado) {
        case PARTIDA_VITORIA:  printf("\n*** VITORIA! Todos os clientes atendidos. ***\n"); break;
        case PARTIDA_DERROTA:  printf("\n*** O cliente perdeu a paciencia. Fim de jogo. ***\n"); break;
        default:               printf("\nVoce saiu da partida.\n"); break;
    }
    int pontos = partidaPontuacao(&p);
    printf("Pontuacao: %d\n\n", pontos);
    partidaDestruir(&p);

    if (!sessaoFimDePartida(s, nome, pontos))
        printf("Aviso: nao foi possivel gravar no historico.\n");
    sessaoRelatorio(s, stdout);
    pausar();
}

static void mostrarHistorico(Sessao *s)
{
    Historico copia;
    CriterioOrdem c = ORDEM_PONTUACAO;
    char cmd[16];

    if (!historicoIniciar(&copia)) return;

    for (;;) {
        double t = sessaoOrdenarHistorico(s, &copia, c);
        if (t < 0) { printf("Erro: falta de memoria.\n"); break; }

        printf("\n========== HISTORICO (%s) ==========\n", historicoNomeCriterio(c));
        int mostrar = copia.n < 20 ? copia.n : 20;
        for (int i = 0; i < mostrar; i++)
            printf("%3d. %-20s %d\n", i + 1, copia.v[i].nome, copia.v[i].pontos);
        if (copia.n > mostrar)
            printf("... (%d entradas no total)\n", copia.n);
        if (copia.n == 0)
            printf("(vazio)\n");
        printf("Merge Sort: %.6f s\n", t);

        printf("\n[a] alfabetica  [p] pontuacao  [Enter] voltar: ");
        if (!lerLinha(cmd, sizeof cmd) || cmd[0] == '\0') break;
        if (cmd[0] == 'a') c = ORDEM_ALFABETICA;
        else if (cmd[0] == 'p') c = ORDEM_PONTUACAO;
    }
    historicoDestruir(&copia);
}

int interfaceExecutar(Sessao *s)
{
    char cmd[16];
    for (;;) {
        printf("\n====== ANTONIO'S - DADO BURGER ======\n"
               "1. Novo jogo\n2. Como jogar\n3. Historico\n0. Sair\n> ");
        if (!lerLinha(cmd, sizeof cmd)) return 0;
        switch (cmd[0]) {
            case '1': novoJogo(s);         break;
            case '2': mostrarComoJogar();  break;
            case '3': mostrarHistorico(s); break;
            case '0': return 0;
            default:  printf("Opcao invalida.\n");
        }
    }
}
