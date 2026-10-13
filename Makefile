# Compila todos os .c do diretorio, equivalente ao comando do Apendice C:
#   gcc -o dado_burger main.c avl.c hash.c heap.c pilha.c fila.c lista.c timer.c -lm -Wall -std=c99

CC      = gcc
CFLAGS  = -Wall -std=c99
LDLIBS  = -lm
SRC     = $(wildcard *.c)
OBJ     = $(SRC:.c=.o)
BIN     = dado_burger

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN) pedidos.txt historico.txt

clean:
	rm -f $(OBJ) $(BIN) medicoes.log

.PHONY: all run clean
