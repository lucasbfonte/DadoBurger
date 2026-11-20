# Dado Burger - INF01203
#
#   make           versao grafica (raylib)       -> ./dado_burger
#   make texto     versao terminal (sem raylib)  -> ./dado_burger
#   make run       compila e roda com pedidos.txt historico.txt
#
# Pasta da raylib pre-compilada (release raylib-5.0_linux_amd64).
# Para usar outra: make RAYLIB=/caminho/da/raylib
RAYLIB ?= $(HOME)/raylib-5.0_linux_amd64

CC      = gcc
CFLAGS  = -Wall -std=c99
BIN     = dado_burger

# Modulos comuns as duas interfaces
COMUM = main.c avl.c hash.c heap.c pilha.c fila.c lista.c timer.c \
        ingredientes.c arquivos.c pedidos.c historico.c partida.c \
        relatorio.c sessao.c textos.c personagens.c

GUI_LIBS = -L$(RAYLIB)/lib -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

all: gui

gui: $(COMUM) interface_raylib.c
	$(CC) $(CFLAGS) -O2 -I$(RAYLIB)/include -o $(BIN) $^ $(GUI_LIBS)

texto: $(COMUM) interface_texto.c
	$(CC) $(CFLAGS) -O2 -o $(BIN) $^ -lm

run: all
	./$(BIN) pedidos.txt historico.txt

clean:
	rm -f *.o $(BIN) medicoes.log

.PHONY: all gui texto run clean
