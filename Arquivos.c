#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"

#define TAM_LINHA 128

/* Conta as linhas lidas para mensagens de erro do tipo "linha 1234". */
static long linha_atual = 0;

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
    while (fgets(buf, TAM_LINHA, fp) != NULL) {
        linha_atual++;
        tirar_fim_de_linha(buf);
        if (buf[0] != '\0')
            return 1;
    }
    return 0;
}

ResultadoLeitura ler_pedido(FILE *fp, int *codigo,
                            char receita[RECEITA_MAX + 1], long *linha_erro)
{
    char buf[TAM_LINHA];

    if (!proxima_linha(fp, buf))
        return LEITURA_FIM;

    /* strtol em vez de atoi: atoi nao avisa se a linha tem lixo
     * ("12a") nem se o numero estoura o int. */
    char *fim;
    errno = 0;
    long valor = strtol(buf, &fim, 10);
    if (fim == buf || *fim != '\0' || errno == ERANGE ||
        valor < INT_MIN || valor > INT_MAX) {
        *linha_erro = linha_atual;
        return LEITURA_ERRO;
        }

    if (!proxima_linha(fp, buf) || !receita_valida(buf)) {
        *linha_erro = linha_atual;
        return LEITURA_ERRO;
    }

    *codigo = (int)valor;
    strcpy(receita, buf);
    return LEITURA_OK;
}
