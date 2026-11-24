# Dado Burger - INF01203
#
#   make           versao grafica (raylib)       -> ./dado_burger
#   make texto     versao terminal (sem raylib)  -> ./dado_burger
#   make run       compila e roda com data/pedidos.txt data/historico.txt
#
# Pasta da raylib pre-compilada (release raylib-5.0_linux_amd64).
# Para usar outra: make RAYLIB=/caminho/da/raylib
RAYLIB ?= $(HOME)/raylib-5.0_linux_amd64

CC      = gcc
CFLAGS  = -Wall -std=c99 -Iinclude
BIN     = dado_burger

# Modulos comuns as duas interfaces
COMUM = src/main.c src/avl.c src/hash.c src/heap.c src/pilha.c src/fila.c \
        src/lista.c src/timer.c src/ingredientes.c src/arquivos.c \
        src/pedidos.c src/historico.c src/partida.c src/relatorio.c \
        src/sessao.c src/textos.c src/personagens.c

GUI_LIBS = -L$(RAYLIB)/lib -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

all: gui

gui: $(COMUM) src/interface_raylib.c
	$(CC) $(CFLAGS) -O2 -I$(RAYLIB)/include -o $(BIN) $^ $(GUI_LIBS)

texto: $(COMUM) src/interface_texto.c
	$(CC) $(CFLAGS) -O2 -o $(BIN) $^ -lm

run: all
	./$(BIN) data/pedidos.txt data/historico.txt

clean:
	rm -f *.o $(BIN) medicoes.log

.PHONY: all gui texto run clean
