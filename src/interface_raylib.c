/* interface_raylib.c
 * Interface grafica (extra da secao 8) feita com raylib 5.0.
 * Visual: lanchonete "antonio's" - parede amarela, clientes do outro
 * lado do balcao e a cozinha (comanda, bandeja e potes) do lado de ca.
 *
 * Organizacao: maquina de estados de telas (MENU, COMO_JOGAR, NOME,
 * PARTIDA, FIM, HISTORICO). A cada frame a tela atual:
 *   1. le mouse/teclado e chama a logica (partida.c / sessao.c);
 *   2. desenha o estado atual, so lendo as estruturas.
 * Nenhuma regra do jogo mora aqui: so desenho e entrada.
 *
 * Fontes (assets/fonts, licenca OFL): Fredoka para titulos e Nunito
 * para o resto. Se a pasta nao for encontrada, usa a fonte da raylib.
 */
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"

#include "config.h"
#include "interface.h"
#include "personagens.h"
#include "textos.h"

#define LARGURA 1280
#define ALTURA   720

/* ------------------------------------------------------------------ */
/*  Paleta                                                              */
/* ------------------------------------------------------------------ */
#define AMARELO        (Color){ 246, 196,  26, 255 }
#define AMARELO_ESC    (Color){ 222, 168,  12, 255 }
#define AMARELO_CLARO  (Color){ 255, 224, 110, 255 }
#define LARANJA        (Color){ 232,  87,  42, 255 }
#define MARROM         (Color){  59,  42,  30, 255 }
#define MARROM2        (Color){  92,  66,  48, 255 }
#define CREME          (Color){ 255, 248, 230, 255 }
#define PAPEL          (Color){ 255, 253, 246, 255 }
#define TETO           (Color){  46,  42,  40, 255 }
#define BALCAO         (Color){ 226, 230, 234, 255 }
#define VIDRO          (Color){ 178, 205, 218, 255 }
#define COZINHA        (Color){  64,  50,  42, 255 }
#define COZINHA2       (Color){  74,  58,  49, 255 }
#define VERDE          (Color){  62, 168,  84, 255 }
#define VERMELHO       (Color){ 212,  58,  44, 255 }
#define CINZA          (Color){ 150, 140, 130, 255 }

/* ------------------------------------------------------------------ */
/*  Estado da interface                                                 */
/* ------------------------------------------------------------------ */
typedef enum {
    TELA_MENU, TELA_COMO_JOGAR, TELA_NOME, TELA_PARTIDA,
    TELA_FIM, TELA_HISTORICO, TELA_SAIR
} Tela;

#define REL_MAX_LINHAS 64
#define REL_TAM_LINHA  96

typedef struct {
    Sessao *s;
    Tela    tela;

    char nome[NOME_MAX + 1];
    int  nome_len;

    Partida partida;
    int     partida_ativa;
    char    msg[160];
    Color   msg_cor;
    float   msg_tempo;
    float   banner;            /* > 0: "Pedido entregue!" */
    int     confirmar_saida;
    float   fim_espera;        /* > 0: partida acabou, mostrando o final */
    int     ultimo_cliente;    /* para o boneco da tela de fim */

    EstadoPartida fim_estado;
    int     fim_pontos;
    int     fim_gravou;
    char    rel[REL_MAX_LINHAS][REL_TAM_LINHA];
    int     rel_n;

    Historico     hist;
    int           hist_ok;
    CriterioOrdem crit;
    double        hist_tempo;
    float         hist_rolagem;
} App;

static int  g_bloquear_cliques = 0;   /* janela modal aberta */
static Font g_titulo, g_texto;

/* ------------------------------------------------------------------ */
/*  Fontes e texto                                                      */
/* ------------------------------------------------------------------ */
static Font carregarFonte(const char *arquivo, int tam) {
    char caminho[512];
    snprintf(caminho, sizeof caminho, "%sassets/fonts/%s", GetApplicationDirectory(), arquivo);
    if (!FileExists(caminho))
        snprintf(caminho, sizeof caminho, "assets/fonts/%s", arquivo);
    if (!FileExists(caminho))
        return GetFontDefault();

    int cps[224];                       /* ASCII + Latin-1 (acentos) */
    for (int i = 0; i < 224; i++) cps[i] = 32 + i;
    Font f = LoadFontEx(caminho, tam, cps, 224);
    GenTextureMipmaps(&f.texture);
    SetTextureFilter(f.texture, TEXTURE_FILTER_TRILINEAR);
    return f;
}

static int ehPadrao(Font f) { return f.texture.id == GetFontDefault().texture.id; }
static float espaco(Font f, float tam) { return ehPadrao(f) ? tam / 10 : 0.5f; }

static float larguraTexto(Font f, const char *t, float tam) {
    return MeasureTextEx(f, t, tam, espaco(f, tam)).x;
}
static void txt(Font f, const char *t, float x, float y, float tam, Color c) {
    DrawTextEx(f, t, (Vector2){ x, y }, tam, espaco(f, tam), c);
}
static void txtCentro(Font f, const char *t, float cx, float y, float tam, Color c) {
    txt(f, t, cx - larguraTexto(f, t, tam) / 2, y, tam, c);
}
static void txtDireita(Font f, const char *t, float xd, float y, float tam, Color c) {
    txt(f, t, xd - larguraTexto(f, t, tam), y, tam, c);
}

/* ------------------------------------------------------------------ */
/*  Componentes                                                         */
/* ------------------------------------------------------------------ */
static int mouseSobre(Rectangle r) {
    return !g_bloquear_cliques && CheckCollisionPointRec(GetMousePosition(), r);
}

static void cartao(Rectangle r, Color c) {
    DrawRectangleRounded((Rectangle){ r.x + 4, r.y + 6, r.width, r.height }, 0.06f, 8,
                         Fade(BLACK, 0.18f));
    DrawRectangleRounded(r, 0.06f, 8, c);
}

/* Botao em pilula. Retorna 1 se clicado neste frame. */
static int botao(Rectangle r, const char *rotulo, float tam, Color fundo, Color letra, int ativo) {
    int sobre = ativo && mouseSobre(r);
    Color f = ativo ? fundo : Fade(fundo, 0.35f);
    if (sobre) f = ColorBrightness(f, 0.15f);
    DrawRectangleRounded((Rectangle){ r.x, r.y + 3, r.width, r.height }, 1.0f, 12,
                         Fade(BLACK, ativo ? 0.22f : 0.08f));
    DrawRectangleRounded(r, 1.0f, 12, f);
    txtCentro(g_texto, rotulo, r.x + r.width / 2, r.y + (r.height - tam) / 2 - 1, tam,
              ativo ? letra : Fade(letra, 0.5f));
    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* Simbolos desenhados (as fontes so tem Latin-1). */
static void simboloCheck(float x, float y, float tam, Color c) {
    DrawLineEx((Vector2){ x, y + tam * 0.55f }, (Vector2){ x + tam * 0.35f, y + tam * 0.9f }, tam * 0.16f, c);
    DrawLineEx((Vector2){ x + tam * 0.35f, y + tam * 0.9f }, (Vector2){ x + tam, y + tam * 0.15f }, tam * 0.16f, c);
}

static void simboloInfinito(float cx, float cy, float tam, Color c) {
    float r = tam * 0.26f, e = tam * 0.09f;
    DrawRing((Vector2){ cx - r, cy }, r - e, r, 0, 360, 24, c);
    DrawRing((Vector2){ cx + r, cy }, r - e, r, 0, 360, 24, c);
}

static void simboloRelogio(float cx, float cy, float r, Color c) {
    DrawRing((Vector2){ cx, cy }, r - 2, r, 0, 360, 20, c);
    DrawLineEx((Vector2){ cx, cy }, (Vector2){ cx, cy - r * 0.6f }, 2, c);
    DrawLineEx((Vector2){ cx, cy }, (Vector2){ cx + r * 0.5f, cy }, 2, c);
}

static void barra(Rectangle r, float f, Color c) {
    DrawRectangleRounded(r, 1.0f, 8, Fade(BLACK, 0.15f));
    if (f > 0)
        DrawRectangleRounded((Rectangle){ r.x, r.y, fmaxf(r.height, r.width * f), r.height },
                             1.0f, 8, c);
}

/* ------------------------------------------------------------------ */
/*  Cenario                                                             */
/* ------------------------------------------------------------------ */
static void teto(float h) {
    DrawRectangle(0, 0, LARGURA, (int) h, TETO);
    for (int i = 0; i < 6; i++) {               /* luminarias */
        float x = 110 + i * 212.0f;
        DrawEllipse((int) x, (int) (h - 8), 46, 6, (Color){ 255, 250, 225, 255 });
        DrawEllipse((int) x, (int) (h - 2), 90, 10, Fade((Color){ 255, 240, 190, 255 }, 0.18f));
    }
}

static void parede(float y0, float y1) {
    DrawRectangleGradientV(0, (int) y0, LARGURA, (int) (y1 - y0), AMARELO, AMARELO_ESC);
}

/* Marca: circulo laranja com um mini-hamburguer. */
static void desenharHamburguer(const Ingrediente *v, int n, float cx, float base,
                               float w, float altura_max);

static void marca(float cx, float cy, float r) {
    static const Ingrediente mini[] = { ING_ALFACE, ING_CARNE, ING_CHEDDAR, ING_PAO_CIMA };
    DrawCircle((int) cx, (int) cy, r, LARANJA);
    DrawCircleLines((int) cx, (int) cy, r - 4, Fade(CREME, 0.5f));
    desenharHamburguer(mini, 4, cx, cy + r * 0.42f, r * 1.15f, r * 1.2f);
}

/* "antonio's / lanches": cx,y = canto esquerdo do texto principal. */
static void letreiro(float x, float y, float tam) {
    marca(x - tam * 0.75f, y + tam * 0.95f, tam * 0.62f);
    txt(g_titulo, "antônio's", x, y, tam, MARROM);
    txt(g_titulo, "lanches", x + tam * 0.06f, y + tam * 0.92f, tam * 0.62f, LARANJA);
}

static void balcao(float y, float h) {
    DrawRectangle(0, (int) y, LARGURA, 14, BALCAO);                      /* tampo */
    DrawRectangle(0, (int) y + 14, LARGURA, 3, Fade(BLACK, 0.12f));
    DrawRectangleGradientV(0, (int) y + 17, LARGURA, (int) h - 17, VIDRO,
                           ColorBrightness(VIDRO, -0.25f));             /* vidro */
    for (int i = 0; i < 9; i++) {                                         /* reflexos */
        float x = i * 160.0f + 30;
        DrawTriangle((Vector2){ x, y + 17 }, (Vector2){ x - 30, y + h },
                     (Vector2){ x + 12, y + 17 }, Fade(WHITE, 0.18f));
    }
}

static void caixaRegistradora(float x, float base, int codigo) {
    DrawRectangleRounded((Rectangle){ x, base - 56, 130, 56 }, 0.2f, 6, (Color){ 70, 70, 76, 255 });
    DrawRectangleRounded((Rectangle){ x + 18, base - 92, 94, 40 }, 0.2f, 6, (Color){ 50, 50, 56, 255 });
    DrawRectangleRounded((Rectangle){ x + 24, base - 86, 82, 26 }, 0.2f, 6, (Color){ 40, 70, 50, 255 });
    txtCentro(g_texto, codigo ? TextFormat("#%d", codigo) : "--", x + 65, base - 82, 15,
              (Color){ 140, 240, 160, 255 });
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            DrawRectangleRounded((Rectangle){ x + 14 + i * 27.0f, base - 46 + j * 18.0f, 21, 12 },
                                 0.4f, 4, (Color){ 120, 120, 128, 255 });
}

/* ------------------------------------------------------------------ */
/*  Ingredientes                                                        */
/* ------------------------------------------------------------------ */
static const char *nomeBonito(Ingrediente ing) {
    static const char *N[ING_TOTAL] = {
        "Pão de cima", "Ketchup", "Mostarda", "Picles", "Alface", "Queijo suíço",
        "Cheddar", "Cebola", "Tomate", "Carne", "Ovo frito", "Bacon"
    };
    return N[ing];
}

static Color corIngrediente(Ingrediente ing) {
    switch (ing) {
        case ING_PAO_CIMA:     return (Color){ 222, 160,  80, 255 };
        case ING_KETCHUP:      return (Color){ 200,  35,  30, 255 };
        case ING_MOSTARDA:     return (Color){ 240, 200,  40, 255 };
        case ING_PICLES:       return (Color){ 105, 150,  55, 255 };
        case ING_ALFACE:       return (Color){ 120, 200,  70, 255 };
        case ING_QUEIJO_SUICO: return (Color){ 250, 232, 160, 255 };
        case ING_CHEDDAR:      return (Color){ 245, 160,  30, 255 };
        case ING_CEBOLA:       return (Color){ 230, 205, 235, 255 };
        case ING_TOMATE:       return (Color){ 225,  60,  50, 255 };
        case ING_CARNE:        return (Color){ 110,  60,  35, 255 };
        case ING_OVO:          return (Color){ 250, 250, 245, 255 };
        case ING_BACON:        return (Color){ 195,  85,  75, 255 };
        default:               return GRAY;
    }
}

static float alturaBase(Ingrediente ing) {
    switch (ing) {
        case ING_PAO_CIMA:  return 44;
        case ING_KETCHUP:
        case ING_MOSTARDA:  return 9;
        case ING_PICLES:    return 12;
        case ING_ALFACE:    return 14;
        case ING_QUEIJO_SUICO:
        case ING_CHEDDAR:   return 11;
        case ING_CEBOLA:    return 11;
        case ING_TOMATE:    return 13;
        case ING_CARNE:     return 26;
        case ING_OVO:       return 16;
        case ING_BACON:     return 12;
        default:            return 10;
    }
}

static float alturaCamada(Ingrediente ing, float w) { return alturaBase(ing) * w / 200.0f; }

/* Camada com base em y=base, centrada em cx, largura w; esc achata. */
static float desenharCamada(Ingrediente ing, float cx, float base, float w, float esc) {
    float h = alturaCamada(ing, w) * esc;
    float x = cx - w / 2, y = base - h;
    float k = w / 200.0f;
    Color c = corIngrediente(ing);

    switch (ing) {
        case ING_PAO_CIMA:
            DrawRectangleRounded((Rectangle){ x, y, w, h }, 0.95f, 16, c);
            DrawRectangle((int) x, (int) (base - h * 0.35f), (int) w, (int) (h * 0.35f), c);
            DrawRectangle((int) x, (int) (base - 3 * k), (int) w, (int) (3 * k + 1),
                          ColorBrightness(c, -0.25f));
            DrawEllipse((int) (x + w * 0.3f), (int) (y + h * 0.3f), w * 0.18f, h * 0.12f,
                        Fade(WHITE, 0.25f));                         /* brilho */
            for (int i = 0; i < 6; i++)
                DrawEllipse((int) (x + w * (0.2f + 0.12f * i)), (int) (y + h * (0.3f + 0.14f * (i % 2))),
                            4 * k, 2 * k, (Color){ 255, 245, 220, 255 });
            break;
        case ING_KETCHUP:
        case ING_MOSTARDA:
            DrawRectangleRounded((Rectangle){ x + w * 0.04f, y, w * 0.92f, h }, 1.0f, 8, c);
            for (int i = 0; i < 4; i++)
                DrawCircle((int) (x + w * (0.18f + 0.22f * i)), (int) base, 3.5f * k, c);
            break;
        case ING_PICLES:
            for (int i = 0; i < 4; i++) {
                float px = x + w * (0.16f + 0.225f * i);
                DrawEllipse((int) px, (int) (y + h / 2), 22 * k, h / 2, c);
                DrawEllipse((int) px, (int) (y + h / 2), 14 * k, h / 3, ColorBrightness(c, 0.25f));
            }
            break;
        case ING_ALFACE:
            for (int i = 0; i <= 10; i++)
                DrawCircle((int) (x - 6 * k + (w + 12 * k) * i / 10.0f), (int) (y + h / 2), h * 0.62f, c);
            DrawRectangle((int) (x - 6 * k), (int) (y + h * 0.3f), (int) (w + 12 * k), (int) (h * 0.5f),
                          ColorBrightness(c, -0.1f));
            break;
        case ING_QUEIJO_SUICO:
            DrawRectangle((int) (x - 4 * k), (int) y, (int) (w + 8 * k), (int) h, c);
            for (int i = 0; i < 5; i++)
                DrawEllipse((int) (x + w * (0.1f + 0.2f * i)), (int) (y + h / 2), 4 * k, h * 0.25f,
                            ColorBrightness(c, -0.15f));
            break;
        case ING_CHEDDAR:
            DrawRectangle((int) (x - 2 * k), (int) y, (int) (w + 4 * k), (int) h, c);
            DrawTriangle((Vector2){ x - 2 * k, base }, (Vector2){ x + 10 * k, base + 12 * k },
                         (Vector2){ x + 22 * k, base }, c);
            DrawTriangle((Vector2){ x + w - 30 * k, base }, (Vector2){ x + w - 16 * k, base + 14 * k },
                         (Vector2){ x + w, base }, c);
            break;
        case ING_CEBOLA:
            for (int i = 0; i < 4; i++) {
                float px = x + w * (0.15f + 0.233f * i);
                DrawEllipse((int) px, (int) (y + h / 2), 26 * k, h / 2, c);
                DrawEllipse((int) px, (int) (y + h / 2), 18 * k, h / 3, (Color){ 170, 120, 180, 255 });
                DrawEllipse((int) px, (int) (y + h / 2), 12 * k, h / 5, c);
            }
            break;
        case ING_TOMATE:
            DrawRectangleRounded((Rectangle){ x + w * 0.03f, y, w * 0.94f, h }, 0.8f, 8, c);
            for (int i = 0; i < 6; i++)
                DrawEllipse((int) (x + w * (0.14f + 0.145f * i)), (int) (y + h / 2), 3 * k, 2 * k,
                            (Color){ 250, 190, 120, 255 });
            break;
        case ING_CARNE:
            DrawRectangleRounded((Rectangle){ x - 4 * k, y, w + 8 * k, h }, 0.7f, 12, c);
            for (int i = 0; i < 5; i++) {
                float gx = x + w * (0.15f + 0.17f * i);
                DrawLineEx((Vector2){ gx, y + h * 0.25f }, (Vector2){ gx + 14 * k, y + h * 0.75f },
                           3 * k, ColorBrightness(c, -0.35f));
            }
            break;
        case ING_OVO:
            DrawEllipse((int) cx, (int) (y + h / 2), w * 0.56f, h / 2, c);
            DrawEllipse((int) (cx + 6 * k), (int) (y + h / 2 - 1), 22 * k, h * 0.42f,
                        (Color){ 250, 190, 40, 255 });
            break;
        case ING_BACON:
            for (int j = 0; j < 2; j++) {
                float yy = y + j * h / 2;
                for (int i = 0; i < 8; i++) {
                    float sx = x + w * i / 8.0f, dy = (i % 2) ? 2 * k : -1 * k;
                    DrawRectangle((int) sx, (int) (yy + dy), (int) (w / 8 + 1), (int) (h / 2),
                                  j ? ColorBrightness(c, -0.12f) : c);
                    DrawRectangle((int) sx, (int) (yy + dy + h / 5), (int) (w / 8 + 1), (int) (h / 9 + 1),
                                  (Color){ 245, 210, 190, 255 });
                }
            }
            break;
        default:
            DrawRectangle((int) x, (int) y, (int) w, (int) h, c);
    }
    return h;
}

/* Icone: molhos e fatias esticam na vertical (ate 3x) para aparecer. */
static void iconeIngrediente(Ingrediente ing, float cx, float cy, float w) {
    float h = alturaCamada(ing, w);
    float esc = h < 12 ? 12 / h : 1.0f;
    if (esc > 3) esc = 3;
    desenharCamada(ing, cx, cy + h * esc / 2, w, esc);
}

static void paoDeBaixo(float cx, float base, float w) {
    Color c = corIngrediente(ING_PAO_CIMA);
    float h = 18 * w / 200.0f;
    DrawRectangleRounded((Rectangle){ cx - w / 2, base - h, w, h }, 0.7f, 12, c);
    DrawRectangle((int) (cx - w / 2), (int) (base - h), (int) w, (int) (3 * w / 200.0f + 1),
                  ColorBrightness(c, -0.25f));
}

static void desenharHamburguer(const Ingrediente *v, int n, float cx, float base,
                               float w, float altura_max) {
    float total = 18 * w / 200.0f;
    for (int i = 0; i < n; i++) total += alturaCamada(v[i], w) + 2;
    float esc = total > altura_max ? altura_max / total : 1.0f;

    paoDeBaixo(cx, base, w);
    float y = base - 18 * w / 200.0f * esc - 2;
    for (int i = 0; i < n; i++)
        y -= desenharCamada(v[i], cx, y, w, esc) + 2 * esc;
}

static int pilhaParaVetor(const PilhaEnc *p, Ingrediente *v, int max) {
    int n = 0;
    for (const nodoPilha *no = p->topo; no && n < max; no = no->prox) n++;
    int i = n;
    for (const nodoPilha *no = p->topo; no && i > 0; no = no->prox) v[--i] = no->dado;
    return n;
}

/* ------------------------------------------------------------------ */
/*  Pessoas                                                             */
/* ------------------------------------------------------------------ */

/* Busto de frente, com a base (peito) em y=base. r = raio da cabeca.
 * humor: 1 feliz ... 0 furioso. sombra: 0..1 escurece (fila ao fundo). */
static void desenharPessoa(const Personagem *p, float cx, float base, float r,
                           float humor, float sombra) {
    Color pele = GetColor(p->pele), cab = GetColor(p->cabelo), roupa = GetColor(p->roupa);
    Color escuro = Fade(BLACK, sombra);
    float cy = base - r * 2.35f;                 /* centro da cabeca */
    Color linha = (Color){ 50, 32, 28, 255 };

    /* cabelo comprido: parte de tras, cai nos ombros */
    if (p->estilo == CABELO_LONGO)
        DrawRectangleRounded((Rectangle){ cx - r * 1.14f, cy - r * 0.9f, r * 2.28f, r * 2.5f },
                             0.6f, 16, cab);

    /* tronco e pescoco */
    DrawRectangleRounded((Rectangle){ cx - r * 1.45f, base - r * 1.15f, r * 2.9f, r * 1.6f },
                         0.55f, 16, roupa);
    DrawRectangle((int) (cx - r * 0.32f), (int) (cy + r * 0.6f), (int) (r * 0.64f), (int) (r * 0.75f), pele);
    if (p->feminino)                                                /* blusa clara */
        DrawTriangle((Vector2){ cx - r * 0.55f, base - r * 1.14f }, (Vector2){ cx, base - r * 0.3f },
                     (Vector2){ cx + r * 0.55f, base - r * 1.14f }, (Color){ 245, 245, 245, 255 });
    DrawTriangle((Vector2){ cx - r * 0.36f, base - r * 1.14f }, (Vector2){ cx, base - r * 0.6f },
                 (Vector2){ cx + r * 0.36f, base - r * 1.14f }, pele);             /* gola V */

    /* orelhas e cabeca */
    DrawCircle((int) (cx - r * 0.98f), (int) (cy + r * 0.12f), r * 0.2f, pele);
    DrawCircle((int) (cx + r * 0.98f), (int) (cy + r * 0.12f), r * 0.2f, pele);
    DrawEllipse((int) cx, (int) cy, r, r * 1.08f, pele);

    /* cabelo: uma "touca" na metade de cima e a testa recortada por uma
     * elipse de pele, o que forma a linha do cabelo */
    if (p->estilo != CABELO_CARECA) {
        DrawCircleSector((Vector2){ cx, cy - r * 0.02f }, r * 1.07f, 180, 360, 32, cab);
        if (p->estilo == CABELO_CACHEADO)
            for (int i = 0; i < 9; i++) {
                float a = PI + i * PI / 8;
                DrawCircle((int) (cx + cosf(a) * r), (int) (cy + sinf(a) * r), r * 0.3f, cab);
            }
        if (p->estilo == CABELO_COQUE)
            DrawCircle((int) cx, (int) (cy - r * 1.12f), r * 0.36f, cab);
        DrawEllipse((int) (cx + r * 0.06f), (int) (cy + r * 0.14f), r * 0.86f, r * 0.62f, pele);
        if (p->estilo == CABELO_CURTO || p->estilo == CABELO_COQUE) {      /* costeletas */
            DrawRectangle((int) (cx - r * 0.98f), (int) (cy - r * 0.2f), (int) (r * 0.16f), (int) (r * 0.42f), cab);
            DrawRectangle((int) (cx + r * 0.82f), (int) (cy - r * 0.2f), (int) (r * 0.16f), (int) (r * 0.42f), cab);
        }
        if (p->estilo == CABELO_LONGO) {                                   /* mechas da frente */
            DrawRectangleRounded((Rectangle){ cx - r * 1.1f, cy - r * 0.35f, r * 0.34f, r * 1.55f }, 0.8f, 8, cab);
            DrawRectangleRounded((Rectangle){ cx + r * 0.76f, cy - r * 0.35f, r * 0.34f, r * 1.55f }, 0.8f, 8, cab);
        }
        if (p->grisalho) {                         /* fios brancos espalhados */
            Color fio = (Color){ 215, 212, 205, 200 };
            for (int i = 0; i < 16; i++) {
                float a = PI * (1.08f + 0.052f * i);
                float rr = r * (0.84f + 0.13f * ((i * 7) % 3) / 2.0f);
                DrawCircle((int) (cx + cosf(a) * rr), (int) (cy + sinf(a) * rr), fmaxf(1.0f, r * 0.045f), fio);
            }
            if (p->estilo == CABELO_CURTO || p->estilo == CABELO_COQUE) {   /* temporas */
                DrawRectangle((int) (cx - r * 0.98f), (int) (cy - r * 0.2f), (int) (r * 0.16f), (int) (r * 0.42f), fio);
                DrawRectangle((int) (cx + r * 0.82f), (int) (cy - r * 0.2f), (int) (r * 0.16f), (int) (r * 0.42f), fio);
            }
        }
    } else {
        DrawEllipse((int) (cx - r * 0.3f), (int) (cy - r * 0.62f), r * 0.25f, r * 0.12f, Fade(WHITE, 0.3f));
    }

    /* barba */
    if (p->barba == BARBA_POR_FAZER) {
        DrawCircleSector((Vector2){ cx, cy + r * 0.1f }, r * 0.97f, 20, 160, 24, Fade(cab, 0.3f));
        DrawEllipse((int) cx, (int) (cy + r * 0.12f), r * 0.74f, r * 0.42f, pele);
    } else if (p->barba == BARBA_CHEIA) {
        DrawCircleSector((Vector2){ cx, cy + r * 0.1f }, r * 1.0f, 10, 170, 24, cab);
        DrawEllipse((int) cx, (int) (cy + r * 0.1f), r * 0.6f, r * 0.34f, pele);
    }

    /* rosto */
    float ex = r * 0.36f, ey = cy + r * 0.02f, bt = fmaxf(1.5f, r * 0.07f);
    float bravo = humor <= 0.3f ? 1.0f : 0.0f;
    DrawEllipse((int) (cx - ex), (int) ey, r * 0.1f, r * 0.13f, linha);
    DrawEllipse((int) (cx + ex), (int) ey, r * 0.1f, r * 0.13f, linha);
    if (p->feminino) {
        DrawLineEx((Vector2){ cx - ex - r * 0.14f, ey - r * 0.08f }, (Vector2){ cx - ex - r * 0.02f, ey - r * 0.1f }, bt, linha);
        DrawLineEx((Vector2){ cx + ex + r * 0.14f, ey - r * 0.08f }, (Vector2){ cx + ex + r * 0.02f, ey - r * 0.1f }, bt, linha);
    }
    /* sobrancelhas (bravas inclinam para dentro) */
    DrawLineEx((Vector2){ cx - ex - r * 0.17f, ey - r * 0.27f },
               (Vector2){ cx - ex + r * 0.16f, ey - r * (0.27f - 0.12f * bravo) }, bt * 1.3f, cab);
    DrawLineEx((Vector2){ cx + ex + r * 0.17f, ey - r * 0.27f },
               (Vector2){ cx + ex - r * 0.16f, ey - r * (0.27f - 0.12f * bravo) }, bt * 1.3f, cab);

    if (p->oculos) {
        DrawRing((Vector2){ cx - ex, ey }, r * 0.22f, r * 0.22f + bt, 0, 360, 24, linha);
        DrawRing((Vector2){ cx + ex, ey }, r * 0.22f, r * 0.22f + bt, 0, 360, 24, linha);
        DrawLineEx((Vector2){ cx - ex + r * 0.22f, ey }, (Vector2){ cx + ex - r * 0.22f, ey }, bt, linha);
    }

    Color boca = p->feminino ? (Color){ 175, 60, 70, 255 } : (Color){ 120, 50, 45, 255 };
    if (humor > 0.6f) {
        DrawRing((Vector2){ cx, cy + r * 0.28f }, r * 0.3f, r * 0.3f + bt, 25, 155, 12, boca);
        DrawCircle((int) (cx - r * 0.55f), (int) (cy + r * 0.38f), r * 0.14f, Fade((Color){ 240, 120, 110, 255 }, 0.35f));
        DrawCircle((int) (cx + r * 0.55f), (int) (cy + r * 0.38f), r * 0.14f, Fade((Color){ 240, 120, 110, 255 }, 0.35f));
    } else if (humor > 0.3f) {
        DrawLineEx((Vector2){ cx - r * 0.22f, cy + r * 0.52f }, (Vector2){ cx + r * 0.22f, cy + r * 0.52f }, bt, boca);
    } else {
        DrawRing((Vector2){ cx, cy + r * 0.82f }, r * 0.28f, r * 0.28f + bt, 205, 335, 12, boca);
        DrawEllipse((int) cx, (int) cy, r, r * 1.08f, Fade(RED, 0.12f));   /* vermelho de raiva */
    }

    if (sombra > 0) {                    /* fila ao fundo fica mais apagada */
        DrawEllipse((int) cx, (int) cy, r * 1.1f, r * 1.15f, escuro);
        DrawRectangleRounded((Rectangle){ cx - r * 1.45f, base - r * 1.15f, r * 2.9f, r * 1.6f },
                             0.55f, 16, escuro);
    }
}

static const char *falaDoCliente(float f, int id) {
    static const char *FELIZ[] = { "Boa tarde!", "Um lanche, por favor!", "Capricha aí!", "Tô com fome!" };
    static const char *MEIO[]  = { "Vai demorar?", "Hmm...", "Tá saindo?", "Tenho aula já já..." };
    static const char *BRAVO[] = { "Anda logo!!", "Que demora!", "Vou embora, hein!", "Sério isso?!" };
    if (f > 0.6f) return FELIZ[id % 4];
    if (f > 0.3f) return MEIO[id % 4];
    return BRAVO[id % 4];
}

static Color corPaciencia(float f) {
    if (f > 0.6f) return VERDE;
    if (f > 0.3f) return (Color){ 240, 170, 20, 255 };
    return VERMELHO;
}

/* Balao de fala apontando para (px, py). */
static void balao(Rectangle r, float px, float py) {
    DrawTriangle((Vector2){ r.x + 30, r.y + r.height - 2 }, (Vector2){ px, py },
                 (Vector2){ r.x + 66, r.y + r.height - 2 }, WHITE);
    DrawRectangleRounded((Rectangle){ r.x + 3, r.y + 5, r.width, r.height }, 0.3f, 10, Fade(BLACK, 0.12f));
    DrawRectangleRounded(r, 0.3f, 10, WHITE);
    DrawTriangle((Vector2){ r.x + 30, r.y + r.height - 2 }, (Vector2){ px, py },
                 (Vector2){ r.x + 66, r.y + r.height - 2 }, WHITE);
}

/* ------------------------------------------------------------------ */
/*  Mensagens e relatorio                                               */
/* ------------------------------------------------------------------ */
static void mensagem(App *a, Color cor, const char *t) {
    snprintf(a->msg, sizeof a->msg, "%.159s", t);
    a->msg_cor = cor;
    a->msg_tempo = 0;
}

/* O relatorio da tela e gerado pela MESMA funcao que grava o log. */
static void capturarRelatorio(App *a) {
    a->rel_n = 0;
    FILE *f = tmpfile();
    if (f == NULL) return;
    sessaoRelatorio(a->s, f);
    rewind(f);
    while (a->rel_n < REL_MAX_LINHAS && fgets(a->rel[a->rel_n], REL_TAM_LINHA, f)) {
        a->rel[a->rel_n][strcspn(a->rel[a->rel_n], "\r\n")] = '\0';
        a->rel_n++;
    }
    fclose(f);
}

/* ------------------------------------------------------------------ */
/*  Tela: MENU                                                          */
/* ------------------------------------------------------------------ */
static void telaMenu(App *a) {
    float t = (float) GetTime();
    teto(44);
    parede(44, 560);
    letreiro(240, 92, 120);
    txt(g_texto, "DADO BURGER  ·  INF01203 Estruturas de Dados", 248, 300, 22, MARROM2);

    const char *op[] = { "Novo jogo", "Como jogar", "Histórico", "Sair" };
    for (int i = 0; i < 4; i++) {
        Rectangle r = { 90, 352 + i * 50.0f, 280, 42 };
        Color c = i == 0 ? LARANJA : MARROM;
        if (botao(r, op[i], 22, c, CREME, 1) || IsKeyPressed(KEY_ONE + i)) {
            if (i == 0)      a->tela = TELA_NOME;
            else if (i == 1) a->tela = TELA_COMO_JOGAR;
            else if (i == 2) a->tela = TELA_HISTORICO;
            else             a->tela = TELA_SAIR;
        }
        txtDireita(g_texto, TextFormat("%d", i + 1), r.x - 14, r.y + 9, 20, MARROM2);
    }
    if (IsKeyPressed(KEY_ESCAPE)) a->tela = TELA_SAIR;

    /* a fila de clientes atras do balcao */
    int n = personagensTotal();
    for (int i = n - 1; i >= 0; i--) {
        float cx = 560 + i * 170.0f;
        float bob = sinf(t * 2.2f + i) * 3, r = 46;
        desenharPessoa(personagemDoCliente(i + 1), cx, 572 + bob, r, 1.0f, 0);
        txtCentro(g_texto, personagemDoCliente(i + 1)->nome, cx, 572 + bob - r * 3.45f - 30,
                  17, MARROM);
    }
    balcao(560, ALTURA - 560);
}

/* ------------------------------------------------------------------ */
/*  Tela: COMO JOGAR                                                    */
/* ------------------------------------------------------------------ */
static void telaComoJogar(App *a) {
    teto(44);
    parede(44, ALTURA);
    txt(g_titulo, "como jogar", 60, 56, 44, MARROM);

    Rectangle c = { 40, 116, LARGURA - 80, ALTURA - 140 };
    cartao(c, PAPEL);

    float y = c.y + 20;
    for (int i = 0; TEXTO_COMO_JOGAR[i] != NULL; i++) {
        const char *l = TEXTO_COMO_JOGAR[i];
        if (l[0] == '#')      { txt(g_titulo, l + 1, c.x + 30, y, 22, LARANJA); y += 28; }
        else if (l[0] == 0)   { y += 6; }
        else                  { txt(g_texto, l, c.x + 46, y, 16, MARROM); y += 21; }
    }

    float yl = c.y + c.height - 66;
    for (int i = 0; i < ING_TOTAL; i++) {
        float cx = c.x + 70 + i * 86.0f;
        iconeIngrediente((Ingrediente) i, cx, yl, 56);
        txtCentro(g_titulo, TextFormat("%c", ingrediente_simbolo((Ingrediente) i)), cx, yl + 14, 20, LARANJA);
        txtCentro(g_texto, nomeBonito((Ingrediente) i), cx, yl + 36, 12, MARROM2);
    }

    if (botao((Rectangle){ LARGURA - 220, 60, 170, 42 }, "Voltar (Esc)", 18, MARROM, CREME, 1) ||
        IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
        a->tela = TELA_MENU;
}

/* ------------------------------------------------------------------ */
/*  Tela: NOME                                                          */
/* ------------------------------------------------------------------ */
static void comecarPartida(App *a) {
    if (a->nome_len == 0) {
        strcpy(a->nome, "Anonimo");
        a->nome_len = (int) strlen(a->nome);
    }
    personagensSortear();
    if (!sessaoNovaPartida(a->s, &a->partida)) {
        mensagem(a, VERMELHO, "Falta de memória para iniciar a partida.");
        return;
    }
    a->partida_ativa = 1;
    a->confirmar_saida = 0;
    a->fim_espera = 0;
    a->banner = 0;
    mensagem(a, CREME, "Monte o pedido de baixo para cima. Bom expediente!");
    a->tela = TELA_PARTIDA;
}

static void telaNome(App *a) {
    int c;
    while ((c = GetCharPressed()) != 0)
        if (c >= 32 && c <= 126 && a->nome_len < NOME_MAX) {
            a->nome[a->nome_len++] = (char) c;
            a->nome[a->nome_len] = '\0';
        }
    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && a->nome_len > 0)
        a->nome[--a->nome_len] = '\0';

    teto(44);
    parede(44, ALTURA);
    letreiro(LARGURA / 2.0f - 150, 90, 80);

    Rectangle card = { LARGURA / 2.0f - 320, 290, 640, 300 };
    cartao(card, PAPEL);
    txtCentro(g_titulo, "Primeiro dia! Qual o seu nome?", LARGURA / 2.0f, card.y + 30, 30, MARROM);
    txtCentro(g_texto, "vai para o crachá e para o histórico", LARGURA / 2.0f, card.y + 72, 16, CINZA);

    Rectangle caixa = { card.x + 60, card.y + 110, card.width - 120, 64 };
    DrawRectangleRounded(caixa, 0.3f, 8, (Color){ 255, 244, 205, 255 });
    DrawRectangleRoundedLines(caixa, 0.3f, 8, 3, AMARELO_ESC);
    txt(g_texto, a->nome, caixa.x + 20, caixa.y + 15, 32, MARROM);
    if (fmod(GetTime(), 1.0) < 0.5)
        DrawRectangle((int) (caixa.x + 24 + larguraTexto(g_texto, a->nome, 32)), (int) caixa.y + 14, 3, 36, MARROM);
    txtDireita(g_texto, TextFormat("%d/%d", a->nome_len, NOME_MAX), caixa.x + caixa.width,
               caixa.y + caixa.height + 6, 14, CINZA);

    if (botao((Rectangle){ card.x + 60, card.y + 220, 240, 48 }, "Voltar (Esc)", 20, MARROM, CREME, 1) ||
        IsKeyPressed(KEY_ESCAPE))
        a->tela = TELA_MENU;
    if (botao((Rectangle){ card.x + card.width - 300, card.y + 220, 240, 48 }, "Abrir o caixa (Enter)",
              20, LARANJA, CREME, 1) || IsKeyPressed(KEY_ENTER))
        comecarPartida(a);
}

/* ------------------------------------------------------------------ */
/*  Tela: PARTIDA                                                       */
/* ------------------------------------------------------------------ */
/* tipo: 0 preparar, 1 adicionar, 2 remover */
static void executarAcao(App *a, int tipo, Ingrediente ing) {
    Partida *p = &a->partida;
    ResultadoAcao r;
    Ingrediente removido = ING_TOTAL;

    if (tipo == 0)      r = partidaPreparar(p, ing);
    else if (tipo == 1) r = partidaAdicionar(p, ing);
    else { pilhaTopo(&p->hamburguer, &removido); r = partidaRemover(p); }

    switch (r) {
        case ACAO_OK:
            if (tipo == 0 && ingrediente_categoria(ing) == CAT_PERECIVEL)
                mensagem(a, CREME, TextFormat("Preparou %s (vence no turno %d).", nomeBonito(ing),
                                              p->turno - 1 + partidaValidade(ing)));
            else if (tipo == 0) mensagem(a, CREME, TextFormat("Preparou %s.", nomeBonito(ing)));
            else if (tipo == 1) mensagem(a, CREME, TextFormat("Adicionou %s.", nomeBonito(ing)));
            else mensagem(a, AMARELO_CLARO, TextFormat("%s foi para o lixo.", nomeBonito(removido)));
            break;
        case ACAO_ATENDEU:
            mensagem(a, (Color){ 150, 230, 160, 255 }, "Pedido certo! Cliente atendido.");
            a->banner = 1.4f;
            break;
        default:
            mensagem(a, (Color){ 255, 140, 120, 255 }, textoResultado(r));
    }

    if (r == ACAO_OK || r == ACAO_ATENDEU || r == ACAO_RECEITA_ERRADA)
        for (int i = 0; i < N_PERECIVEIS; i++)
            if (p->vencidos_no_turno[i]) {
                char extra[320];
                snprintf(extra, sizeof extra, "%s  ·  %d %s venceu e foi descartado(a)!", a->msg,
                         p->vencidos_no_turno[i], nomeBonito(partidaPerecivelDoIndice(i)));
                mensagem(a, AMARELO_CLARO, extra);
            }
}

static void registrarFim(App *a) {
    Cliente *c = filaFrente(&a->partida.fila);
    a->ultimo_cliente = c ? c->id : a->partida.total_clientes;
    a->fim_estado = a->partida.estado;
    a->fim_pontos = partidaPontuacao(&a->partida);
    a->fim_gravou = sessaoFimDePartida(a->s, a->nome, a->fim_pontos);
    capturarRelatorio(a);
}

static void lerEntradaPartida(App *a) {
    if (a->confirmar_saida) {
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_ENTER)) { a->confirmar_saida = 0; partidaDesistir(&a->partida); }
        else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) a->confirmar_saida = 0;
        return;
    }
    if (a->fim_espera > 0) return;

    if (IsKeyPressed(KEY_ESCAPE)) { a->confirmar_saida = 1; return; }
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) executarAcao(a, 2, ING_TOTAL);

    int shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    for (int i = 0; i < ING_TOTAL; i++) {
        char sim = ingrediente_simbolo((Ingrediente) i);
        if (IsKeyPressed(KEY_A + (sim - 'a')))
            executarAcao(a, shift ? 0 : 1, (Ingrediente) i);
    }
}

#define Y_BALCAO 356.0f    /* tampo do balcao */
#define Y_COZINHA 404.0f   /* comeco da area de trabalho */

static void desenharSalao(App *a) {
    Partida *p = &a->partida;
    float t = (float) GetTime();

    teto(44);
    parede(44, Y_BALCAO);
    letreiro(960, 60, 66);

    /* HUD no teto */
    txt(g_texto, TextFormat("Turno %d", p->turno), 24, 10, 22, CREME);
    txt(g_texto, TextFormat("Atendidos %d/%d", p->atendidos, p->total_clientes), 150, 10, 22, CREME);
    txt(g_texto, TextFormat("Caixa: %s", a->nome), 350, 12, 18, Fade(CREME, 0.6f));
    if (botao((Rectangle){ LARGURA - 150, 6, 136, 32 }, "Sair (Esc)", 16, VERMELHO, CREME,
              a->fim_espera <= 0))
        a->confirmar_saida = 1;

    /* fila ao fundo (da ultima para a primeira, para sobrepor certo) */
    Cliente *frente = filaFrente(&p->fila);
    int esperando = filaTamanho(&p->fila) - (frente ? 1 : 0);
    const nodoFila *visiveis[6];
    int nv = 0;
    for (const nodoFila *no = p->fila.ini ? p->fila.ini->prox : NULL; no && nv < 6; no = no->prox)
        visiveis[nv++] = no;
    for (int i = nv - 1; i >= 0; i--) {
        float r = 30 - i * 2.0f;
        float cx = 560 + i * 72.0f;
        float bob = sinf(t * 1.6f + i * 1.3f) * 1.5f;
        desenharPessoa(personagemDoCliente(visiveis[i]->dado.id), cx, Y_BALCAO + 8 + bob, r, 1.0f,
                       0.08f + 0.04f * i);
    }
    if (esperando > nv)
        txt(g_texto, TextFormat("+%d", esperando - nv), 560 + nv * 72.0f - 20, Y_BALCAO - 50, 22, MARROM2);

    /* cliente no caixa */
    if (frente) {
        const Personagem *pe = personagemDoCliente(frente->id);
        float f = (float) frente->paciencia / p->paciencia_max;
        float tremor = (f <= 0.3f && a->fim_espera <= 0) ? sinf(t * 40) * 2.5f : 0;
        float cx = 330 + tremor, r = 58;
        desenharPessoa(pe, cx, Y_BALCAO + 10, r, f, 0);

        Rectangle b = { 402, 62, 300, 108 };
        balao(b, cx + r * 0.9f, Y_BALCAO - r * 2.6f);
        txt(g_titulo, pe->nome, b.x + 18, b.y + 10, 26, MARROM);
        txtDireita(g_texto, TextFormat("cliente %d", frente->id), b.x + b.width - 16, b.y + 16, 14, CINZA);
        txt(g_texto, falaDoCliente(f, frente->id), b.x + 18, b.y + 42, 18, MARROM2);
        barra((Rectangle){ b.x + 18, b.y + 76, b.width - 90, 14 }, f, corPaciencia(f));
        txtDireita(g_texto, TextFormat("%d/%d", frente->paciencia, p->paciencia_max),
                   b.x + b.width - 16, b.y + 72, 16, MARROM);
    }

    balcao(Y_BALCAO, Y_COZINHA - Y_BALCAO);
    caixaRegistradora(120, Y_BALCAO + 2, frente ? frente->codigo : 0);
}

static void desenharComanda(App *a) {
    Partida *p = &a->partida;
    Cliente *c = filaFrente(&p->fila);
    Rectangle r = { 22, Y_COZINHA + 18, 252, ALTURA - Y_COZINHA - 32 };

    DrawRectangle((int) r.x + 4, (int) r.y + 6, (int) r.width, (int) r.height, Fade(BLACK, 0.25f));
    DrawRectangleRec(r, PAPEL);
    DrawRectangle((int) r.x, (int) r.y, (int) r.width, 8, LARANJA);
    DrawCircle((int) (r.x + r.width / 2), (int) r.y + 4, 6, (Color){ 120, 120, 128, 255 });  /* pino */
    if (c == NULL) return;

    txt(g_titulo, TextFormat("Pedido #%d", c->codigo), r.x + 14, r.y + 16, 22, MARROM);
    txt(g_texto, "de baixo para cima", r.x + 14, r.y + 40, 13, CINZA);

    Ingrediente monte[64];
    int m = pilhaParaVetor(&p->hamburguer, monte, 64);
    int n = (int) strlen(c->receita), certos = 0;
    while (certos < m && certos < n &&
           monte[certos] == (Ingrediente) ingrediente_de_simbolo(c->receita[certos]))
        certos++;

    float lh = 27, base = r.y + r.height - 32;
    txt(g_texto, "pão de baixo (já no prato)", r.x + 16, base + 8, 13, CINZA);
    for (int i = 0; i < n; i++) {
        Ingrediente ing = (Ingrediente) ingrediente_de_simbolo(c->receita[i]);
        float y = base - (i + 1) * lh;
        int feito = i < certos, proximo = (i == certos && m == certos);

        if (proximo)
            DrawRectangleRounded((Rectangle){ r.x + 6, y, r.width - 12, lh - 2 }, 0.4f, 6,
                                 (Color){ 255, 236, 170, 255 });
        iconeIngrediente(ing, r.x + 30, y + lh / 2 - 1, 34);
        txt(g_titulo, TextFormat("%c", ingrediente_simbolo(ing)), r.x + 56, y + 3, 17, LARANJA);
        txt(g_texto, nomeBonito(ing), r.x + 76, y + 5, 15, feito ? CINZA : MARROM);
        if (feito)
            DrawLineEx((Vector2){ r.x + 76, y + 13 }, (Vector2){ r.x + 76 + larguraTexto(g_texto, nomeBonito(ing), 15), y + 13 },
                       2, Fade(CINZA, 0.8f));
        if (feito)       simboloCheck(r.x + r.width - 30, y + 5, 16, VERDE);
        else if (i < m)  txtDireita(g_titulo, "x", r.x + r.width - 16, y + 1, 22, VERMELHO);
    }
}

static void desenharBandeja(App *a) {
    Partida *p = &a->partida;
    float cx = 530, base = ALTURA - 40;

    /* bandeja vermelha com papel */
    DrawRectangleRounded((Rectangle){ cx - 190, base - 22, 380, 44 }, 0.5f, 10, (Color){ 176, 48, 36, 255 });
    DrawRectangleRounded((Rectangle){ cx - 176, base - 16, 352, 30 }, 0.5f, 10, (Color){ 200, 62, 46, 255 });
    DrawRectangleRounded((Rectangle){ cx - 120, base - 14, 240, 22 }, 0.3f, 6, (Color){ 250, 238, 210, 255 });

    Ingrediente monte[64];
    int m = pilhaParaVetor(&p->hamburguer, monte, 64);
    desenharHamburguer(monte, m, cx, base, 220, base - Y_COZINHA - 90);

    Ingrediente topo;
    int pode = pilhaTopo(&p->hamburguer, &topo) && a->fim_espera <= 0;
    if (botao((Rectangle){ 620, Y_COZINHA + 64, 150, 34 }, "Remover topo", 15, MARROM2, CREME, pode))
        executarAcao(a, 2, ING_TOTAL);

    /* mensagem + aviso de ingrediente errado */
    float alfa = a->msg_tempo < 4.0f ? 1.0f : 0.6f;
    txtCentro(g_texto, a->msg, cx, Y_COZINHA + 22, 18, Fade(a->msg_cor, alfa));
    Cliente *c = filaFrente(&p->fila);
    if (c) {
        int certos = 0, n = (int) strlen(c->receita);
        while (certos < m && certos < n &&
               monte[certos] == (Ingrediente) ingrediente_de_simbolo(c->receita[certos]))
            certos++;
        if (m > certos)
            txt(g_texto, "Tem ingrediente errado na bandeja:\nremova o topo até sumir o x.",
                300, Y_COZINHA + 66, 15, (Color){ 255, 140, 120, 255 });
    }

    if (a->banner > 0) {
        float al = a->banner > 1.0f ? 1.0f : a->banner;
        Rectangle b = { cx - 170, Y_COZINHA + 120, 340, 64 };
        DrawRectangleRounded(b, 1.0f, 12, Fade(VERDE, al));
        txtCentro(g_titulo, "Pedido entregue!", cx, b.y + 14, 32, Fade(WHITE, al));
    }
}

static void desenharPotes(App *a) {
    Partida *p = &a->partida;
    Rectangle area = { 790, Y_COZINHA + 18, 470, ALTURA - Y_COZINHA - 32 };
    Ingrediente topo;
    int fechado = pilhaTopo(&p->hamburguer, &topo) && topo == ING_PAO_CIMA;
    int livre = a->fim_espera <= 0;

    txt(g_texto, "clique: adicionar   ·   + : preparar   ·   teclado: letra / Shift+letra",
        area.x, area.y - 16, 12, Fade(CREME, 0.6f));

    float gw = 8, w = (area.width - 3 * gw) / 4, h = (area.height - 2 * gw - 4) / 3;
    for (int i = 0; i < ING_TOTAL; i++) {
        Ingrediente ing = (Ingrediente) i;
        Categoria cat = ingrediente_categoria(ing);
        int q = partidaQuantidade(p, ing);
        Rectangle r = { area.x + (i % 4) * (w + gw), area.y + 4 + (i / 4) * (h + gw), w, h };

        int pode_add = livre && !fechado && q != 0;
        Rectangle bprep = { r.x + 6, r.y + 6, 26, 26 };
        int sobre_prep = cat != CAT_SEM_PREPARO && mouseSobre(bprep);
        int sobre = mouseSobre(r) && !sobre_prep;

        Color fundo = q == 0 ? (Color){ 232, 222, 205, 255 } : CREME;
        if (sobre && pode_add) fundo = (Color){ 255, 236, 170, 255 };
        DrawRectangleRounded((Rectangle){ r.x, r.y + 3, r.width, r.height }, 0.2f, 8, Fade(BLACK, 0.3f));
        DrawRectangleRounded(r, 0.2f, 8, fundo);

        iconeIngrediente(ing, r.x + r.width / 2, r.y + 24, 56);
        txtCentro(g_texto, nomeBonito(ing), r.x + r.width / 2, r.y + 38, 12, MARROM2);
        txt(g_titulo, TextFormat("%c", ingrediente_simbolo(ing)), r.x + 10, r.y + h - 30, 22, LARANJA);
        if (q < 0)
            simboloInfinito(r.x + r.width - 24, r.y + h - 19, 26, MARROM);
        else
            txtDireita(g_texto, TextFormat("%d", q), r.x + r.width - 10, r.y + h - 31, 22,
                       q == 0 ? VERMELHO : MARROM);

        /* validade do proximo perecivel */
        int venc;
        if (partidaProximoVencimento(p, ing, &venc)) {
            int falta = venc - p->turno;
            Color cb = falta <= 2 ? VERMELHO : (Color){ 240, 150, 20, 255 };
            Rectangle tag = { r.x + r.width - 50, r.y + 6, 44, 20 };
            DrawRectangleRounded(tag, 1.0f, 8, cb);
            simboloRelogio(tag.x + 12, tag.y + 10, 7, WHITE);
            txtCentro(g_texto, TextFormat("%d", falta), tag.x + 30, tag.y + 2, 14, WHITE);
        }

        if (cat != CAT_SEM_PREPARO) {
            int ativo = livre;
            Color cp = sobre_prep && ativo ? ColorBrightness(LARANJA, 0.15f) : LARANJA;
            DrawCircle((int) (bprep.x + 13), (int) (bprep.y + 13), 13, ativo ? cp : Fade(LARANJA, 0.4f));
            txtCentro(g_titulo, "+", bprep.x + 13, bprep.y + 1, 24, WHITE);
            if (sobre_prep && ativo && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                executarAcao(a, 0, ing);
        }

        if (sobre && pode_add && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            executarAcao(a, 1, ing);
        if (sobre && cat != CAT_SEM_PREPARO && livre && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            executarAcao(a, 0, ing);
    }
}

static void desenharModal(const char *titulo, const char *sub, Color cor) {
    DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.55f));
    Rectangle r = { LARGURA / 2.0f - 270, ALTURA / 2.0f - 110, 540, 220 };
    cartao(r, PAPEL);
    txtCentro(g_titulo, titulo, r.x + r.width / 2, r.y + 30, 36, cor);
    if (sub) txtCentro(g_texto, sub, r.x + r.width / 2, r.y + 80, 17, CINZA);
}

static void telaPartida(App *a) {
    Partida *p = &a->partida;
    float dt = GetFrameTime();
    a->msg_tempo += dt;
    if (a->banner > 0) a->banner -= dt;

    lerEntradaPartida(a);

    if (!partidaEmAndamento(p) && a->fim_espera <= 0) {   /* acabou agora */
        registrarFim(a);
        a->fim_espera = 1.6f;
    }

    g_bloquear_cliques = a->confirmar_saida || a->fim_espera > 0;
    ClearBackground(COZINHA);
    for (int i = 0; i < 12; i++)                          /* azulejos da cozinha */
        DrawRectangle(0, (int) (Y_COZINHA + i * 28), LARGURA, 14, COZINHA2);
    desenharSalao(a);
    desenharComanda(a);
    desenharBandeja(a);
    desenharPotes(a);
    g_bloquear_cliques = 0;

    if (a->confirmar_saida) {
        desenharModal("Fechar o caixa?", "a pontuação atual vai para o histórico", MARROM);
        Rectangle r = { LARGURA / 2.0f - 270, ALTURA / 2.0f - 110, 540, 220 };
        if (botao((Rectangle){ r.x + 50, r.y + 136, 200, 48 }, "Sim (S)", 20, VERMELHO, CREME, 1)) {
            a->confirmar_saida = 0;
            partidaDesistir(p);
        }
        if (botao((Rectangle){ r.x + 290, r.y + 136, 200, 48 }, "Não (N)", 20, VERDE, CREME, 1))
            a->confirmar_saida = 0;
    }

    if (a->fim_espera > 0) {
        const char *t; Color c;
        switch (p->estado) {
            case PARTIDA_VITORIA: t = "Todos atendidos!";        c = VERDE;    break;
            case PARTIDA_DERROTA: t = "O cliente foi embora...";  c = VERMELHO; break;
            default:              t = "Caixa fechado";           c = MARROM;   break;
        }
        DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.35f));
        DrawRectangle(0, ALTURA / 2 - 60, LARGURA, 110, Fade(AMARELO, 0.95f));
        txtCentro(g_titulo, t, LARGURA / 2.0f, ALTURA / 2.0f - 40, 64, c);
        a->fim_espera -= dt;
        if (a->fim_espera <= 0) {
            partidaDestruir(p);
            a->partida_ativa = 0;
            a->tela = TELA_FIM;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Tela: FIM                                                           */
/* ------------------------------------------------------------------ */
static void desenharRelatorio(App *a, Rectangle r) {
    cartao(r, PAPEL);
    float y = r.y + 18;
    for (int i = 0; i < a->rel_n; i++) {
        const char *l = a->rel[i];
        if (l[0] == '\0') { y += 4; continue; }
        if (l[0] == '=') {
            if (strstr(l, "RELATORIO")) { txt(g_titulo, "Relatório de desempenho", r.x + 22, y, 26, MARROM); y += 36; }
            continue;
        }
        if (l[0] == '-' && l[1] == '-') {
            char sec[REL_TAM_LINHA];
            snprintf(sec, sizeof sec, "%s", l + 3);
            size_t n = strlen(sec);
            while (n > 0 && (sec[n - 1] == '-' || sec[n - 1] == ' ')) sec[--n] = '\0';
            txt(g_titulo, sec, r.x + 22, y, 18, LARANJA);
            y += 24;
            continue;
        }
        const char *dp = strchr(l, ':');
        if (dp) {
            char rot[REL_TAM_LINHA];
            int n = (int) (dp - l);
            memcpy(rot, l, n);
            rot[n] = '\0';
            const char *val = dp + 1;
            while (*val == ' ') val++;
            txt(g_texto, rot, r.x + 36, y, 15, MARROM2);
            txtDireita(g_texto, val, r.x + r.width - 30, y, 15, MARROM);
            DrawLine((int) (r.x + 36), (int) (y + 18), (int) (r.x + r.width - 30), (int) (y + 18),
                     Fade(CINZA, 0.2f));
        } else {
            txt(g_texto, l, r.x + 36, y, 15, MARROM);
        }
        y += 20;
    }
}

static void telaFim(App *a) {
    teto(44);
    parede(44, ALTURA);

    const char *t; Color c; float humor;
    switch (a->fim_estado) {
        case PARTIDA_VITORIA: t = "Expediente perfeito!"; c = VERDE;    humor = 1.0f;  break;
        case PARTIDA_DERROTA: t = "Fim de expediente";    c = VERMELHO; humor = 0.0f;  break;
        default:              t = "Caixa fechado";        c = MARROM;   humor = 0.45f; break;
    }

    Rectangle esq = { 40, 70, 470, ALTURA - 100 };
    cartao(esq, PAPEL);
    float cx = esq.x + esq.width / 2;
    txtCentro(g_titulo, t, cx, esq.y + 24, 36, c);
    BeginScissorMode((int) esq.x, (int) esq.y, (int) esq.width, 258);
    desenharPessoa(personagemDoCliente(a->ultimo_cliente), cx, esq.y + 268, 46, humor, 0);
    EndScissorMode();
    DrawRectangle((int) esq.x + 30, (int) (esq.y + 258), (int) esq.width - 60, 6, BALCAO);
    txtCentro(g_texto, a->nome, cx, esq.y + 298, 24, MARROM);
    txtCentro(g_titulo, TextFormat("%d", a->fim_pontos), cx, esq.y + 326, 96, LARANJA);
    txtCentro(g_texto, a->fim_pontos == 1 ? "cliente atendido" : "clientes atendidos", cx,
              esq.y + 428, 20, MARROM2);
    txtCentro(g_texto, a->fim_gravou ? "gravado no histórico e no medicoes.log"
                                     : "ERRO ao gravar no histórico!",
              cx, esq.y + 458, 14, a->fim_gravou ? CINZA : VERMELHO);

    if (botao((Rectangle){ esq.x + 30, esq.y + esq.height - 72, 190, 46 }, "Menu (Esc)", 18,
              MARROM, CREME, 1) || IsKeyPressed(KEY_ESCAPE))
        a->tela = TELA_MENU;
    if (botao((Rectangle){ esq.x + 250, esq.y + esq.height - 72, 190, 46 }, "De novo (Enter)", 18,
              LARANJA, CREME, 1) || IsKeyPressed(KEY_ENTER))
        comecarPartida(a);

    desenharRelatorio(a, (Rectangle){ 540, 70, LARGURA - 580, ALTURA - 100 });
}

/* ------------------------------------------------------------------ */
/*  Tela: HISTORICO                                                     */
/* ------------------------------------------------------------------ */
static void ordenarHistorico(App *a, CriterioOrdem c) {
    a->crit = c;
    a->hist_tempo = sessaoOrdenarHistorico(a->s, &a->hist, c);
    a->hist_rolagem = 0;
}

static void telaHistorico(App *a) {
    if (!a->hist_ok) {
        if (!historicoIniciar(&a->hist)) { a->tela = TELA_MENU; return; }
        a->hist_ok = 1;
        ordenarHistorico(a, ORDEM_PONTUACAO);
    }

    teto(44);
    parede(44, ALTURA);
    txt(g_titulo, "histórico", 60, 56, 44, MARROM);

    int pts = a->crit == ORDEM_PONTUACAO;
    if (botao((Rectangle){ 300, 64, 190, 38 }, "Pontuação (P)", 17, pts ? LARANJA : Fade(MARROM, 0.5f), CREME, 1) ||
        IsKeyPressed(KEY_P))
        ordenarHistorico(a, ORDEM_PONTUACAO);
    if (botao((Rectangle){ 500, 64, 190, 38 }, "Alfabética (A)", 17, !pts ? LARANJA : Fade(MARROM, 0.5f), CREME, 1) ||
        IsKeyPressed(KEY_A))
        ordenarHistorico(a, ORDEM_ALFABETICA);
    if (botao((Rectangle){ LARGURA - 220, 64, 170, 38 }, "Voltar (Esc)", 17, MARROM, CREME, 1) ||
        IsKeyPressed(KEY_ESCAPE)) {
        historicoDestruir(&a->hist);
        a->hist_ok = 0;
        a->tela = TELA_MENU;
        return;
    }

    Rectangle tab = { 60, 120, LARGURA - 120, ALTURA - 170 };
    cartao(tab, PAPEL);
    float lh = 30;
    int visiveis = (int) ((tab.height - 60) / lh);

    float max_rol = (float) (a->hist.n > visiveis ? a->hist.n - visiveis : 0);
    a->hist_rolagem -= GetMouseWheelMove() * 3;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) a->hist_rolagem += 1;
    if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP))     a->hist_rolagem -= 1;
    if (IsKeyPressed(KEY_PAGE_DOWN)) a->hist_rolagem += visiveis;
    if (IsKeyPressed(KEY_PAGE_UP))   a->hist_rolagem -= visiveis;
    if (IsKeyPressed(KEY_HOME))      a->hist_rolagem = 0;
    if (IsKeyPressed(KEY_END))       a->hist_rolagem = max_rol;
    if (a->hist_rolagem < 0) a->hist_rolagem = 0;
    if (a->hist_rolagem > max_rol) a->hist_rolagem = max_rol;

    float colPos = tab.x + 40, colNome = tab.x + 150, colPts = tab.x + 620;
    txt(g_titulo, "#", colPos, tab.y + 16, 20, LARANJA);
    txt(g_titulo, "Nome", colNome, tab.y + 16, 20, LARANJA);
    txt(g_titulo, "Clientes atendidos", colPts, tab.y + 16, 20, LARANJA);
    DrawRectangle((int) tab.x + 20, (int) tab.y + 46, (int) tab.width - 40, 2, AMARELO);

    if (a->hist.n == 0)
        txtCentro(g_texto, "Nenhuma partida registrada ainda.", tab.x + tab.width / 2, tab.y + 110, 20, CINZA);

    int ini = (int) a->hist_rolagem;
    for (int k = 0; k < visiveis && ini + k < a->hist.n; k++) {
        const Entrada *e = &a->hist.v[ini + k];
        float y = tab.y + 54 + k * lh;
        if ((ini + k) % 2 == 0)
            DrawRectangleRounded((Rectangle){ tab.x + 20, y - 3, tab.width - 70, lh }, 0.4f, 6,
                                 (Color){ 255, 243, 200, 255 });
        int meu = a->nome_len > 0 && strcmp(e->nome, a->nome) == 0;
        Color ct = meu ? LARANJA : MARROM;
        txt(g_texto, TextFormat("%d", ini + k + 1), colPos, y + 2, 17, CINZA);
        txt(g_texto, e->nome, colNome, y + 2, 17, ct);
        txt(g_texto, TextFormat("%d", e->pontos), colPts, y + 2, 17, ct);
    }

    if (max_rol > 0) {
        Rectangle trilho = { tab.x + tab.width - 30, tab.y + 56, 10, tab.height - 72 };
        DrawRectangleRounded(trilho, 1, 6, (Color){ 240, 228, 200, 255 });
        float alt = fmaxf(30, trilho.height * visiveis / a->hist.n);
        float pos = trilho.y + (trilho.height - alt) * (a->hist_rolagem / max_rol);
        DrawRectangleRounded((Rectangle){ trilho.x, pos, trilho.width, alt }, 1, 6, AMARELO_ESC);
    }

    const char *info = a->hist_tempo >= 0
        ? TextFormat("%d entradas  ·  Merge Sort: %.6f s (gravado no medicoes.log)  ·  role com a roda do mouse, setas, PgUp/PgDn",
                     a->hist.n, a->hist_tempo)
        : "Falta de memória para ordenar.";
    txt(g_texto, info, 62, ALTURA - 38, 15, MARROM);
}

/* ------------------------------------------------------------------ */
/*  Laco principal                                                      */
/* ------------------------------------------------------------------ */
int interfaceExecutar(Sessao *s) {
    static App app;
    App *a = &app;
    memset(a, 0, sizeof *a);
    a->s = s;
    a->tela = TELA_MENU;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(LARGURA, ALTURA, "antônio's - Dado Burger");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    g_titulo = carregarFonte("Fredoka-SemiBold.ttf", 96);
    g_texto  = carregarFonte("Nunito-ExtraBold.ttf", 48);

    while (a->tela != TELA_SAIR) {
        if (WindowShouldClose()) {
            if (a->partida_ativa && partidaEmAndamento(&a->partida)) {
                partidaDesistir(&a->partida);   /* conta como saida voluntaria */
                registrarFim(a);
            }
            break;
        }

        BeginDrawing();
        ClearBackground(AMARELO);
        switch (a->tela) {
            case TELA_MENU:       telaMenu(a);      break;
            case TELA_COMO_JOGAR: telaComoJogar(a); break;
            case TELA_NOME:       telaNome(a);      break;
            case TELA_PARTIDA:    telaPartida(a);   break;
            case TELA_FIM:        telaFim(a);       break;
            case TELA_HISTORICO:  telaHistorico(a); break;
            case TELA_SAIR:                         break;
        }
        EndDrawing();
    }

    if (a->partida_ativa) partidaDestruir(&a->partida);
    if (a->hist_ok)       historicoDestruir(&a->hist);
    if (!ehPadrao(g_titulo)) UnloadFont(g_titulo);
    if (!ehPadrao(g_texto))  UnloadFont(g_texto);
    CloseWindow();
    return 0;
}
