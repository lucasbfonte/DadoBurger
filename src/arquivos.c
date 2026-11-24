#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"

#define TAM_LINHA 128

/* Conta as linhas lidas para mensagens de erro do tipo "linha 1234".
 * Zera sozinho quando o arquivo lido muda (pedidos -> historico). */
static long  linha_atual = 0;
static FILE *arquivo_atual = NULL;

/* Remove \n e \r do fim. O \r aparece quando o arquivo foi gerado no
 * Windows (fim de linha "\r\n") e quebraria a validacao da receita. */
static void tirar_fim_de_linha(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
        s[--n] = '\0';
}

/* Le a proxima linha nao vazia. Retorna 0 no fim do arquivo. */
static int proxima_linha(FILE *fp, char *buf)
{
    if (fp != arquivo_atual) {
        arquivo_atual = fp;
        linha_atual = 0;
    }
    while (fgets(buf, TAM_LINHA, fp) != NULL) {
        linha_atual++;
        tirar_fim_de_linha(buf);
        if (buf[0] != '\0')
            return 1;
    }
    return 0;
}

/* Converte a linha inteira em int. Retorna 0 se tiver lixo ou estourar. */
static int linha_para_int(const char *buf, int *out)
{
    /* strtol em vez de atoi: atoi nao avisa se a linha tem lixo
     * ("12a") nem se o numero estoura o int. */
    char *fim;
    errno = 0;
    long valor = strtol(buf, &fim, 10);
    if (fim == buf || *fim != '\0' || errno == ERANGE ||
        valor < INT_MIN || valor > INT_MAX)
        return 0;
    *out = (int)valor;
    return 1;
}

ResultadoLeitura ler_pedido(FILE *fp, int *codigo,
                            char receita[RECEITA_MAX + 1], long *linha_erro)
{
    char buf[TAM_LINHA];

    if (!proxima_linha(fp, buf))
        return LEITURA_FIM;

    if (!linha_para_int(buf, codigo)) {
        *linha_erro = linha_atual;
        return LEITURA_ERRO;
    }

    if (!proxima_linha(fp, buf) || !receita_valida(buf)) {
        *linha_erro = linha_atual;
        return LEITURA_ERRO;
    }

    strcpy(receita, buf);
    return LEITURA_OK;
}

ResultadoLeitura ler_historico(FILE *fp, char nome[NOME_MAX + 1],
                               int *pontos, long *linha_erro)
{
    char buf[TAM_LINHA];

    if (!proxima_linha(fp, buf))
        return LEITURA_FIM;

    strncpy(nome, buf, NOME_MAX);
    nome[NOME_MAX] = '\0';

    if (!proxima_linha(fp, buf) || !linha_para_int(buf, pontos)) {
        *linha_erro = linha_atual;
        return LEITURA_ERRO;
    }
    return LEITURA_OK;
}

int gravar_historico(const char *arquivo, const char *nome, int pontos)
{
    FILE *f = fopen(arquivo, "a");
    if (f == NULL)
        return 0;
    int ok = fprintf(f, "%s\n%d\n", nome, pontos) > 0;
    if (fclose(f) != 0)
        ok = 0;
    return ok;
}
