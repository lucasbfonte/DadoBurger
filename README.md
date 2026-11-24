# 🍔 Dado Burger

Jogo de cozinha em C, feito como trabalho final de **Estruturas de Dados**
(INF01203, UFRGS, 2026/2). Você comanda a cozinha do *Antônio's* e precisa
montar os hambúrgueres de uma fila de clientes antes que a paciência de cada
um acabe. Cada parte do jogo é movida por uma estrutura de dados estudada na
disciplina.

Tem duas interfaces sobre a **mesma lógica**: uma de terminal (sem
dependências) e uma gráfica feita com [raylib](https://www.raylib.com/).

<!--
Capturas de tela: coloque os arquivos em uma pasta (por exemplo docs/img/) e descomente.

![Menu](docs/img/menu.png)
![Partida](docs/img/partida.png)
-->

## Como jogar

Cada cliente pede um hambúrguer descrito por uma sequência de letras, de
baixo para cima (o pão de baixo já está no prato; o pedido termina com o pão
de cima, `z`). A cada turno você faz **uma** ação:

- **preparar** um ingrediente (+1 no estoque);
- **adicionar** um ingrediente ao hambúrguer;
- **remover** o topo do hambúrguer (vai para o lixo).

O cliente da frente perde 1 de paciência a cada turno (começa com 16). Se
chegar a zero, fim de jogo. Atenda os 10 clientes para vencer; a pontuação é o
número de clientes atendidos.

| Tipo | Símbolo | Ingrediente | Validade |
|---|---|---|---|
| Sem preparo (infinitos) | `z` `k` `m` `p` `a` | pão de cima, ketchup, mostarda, picles, alface | n/a |
| Com preparo | `s` `q` `c` `t` | queijo suíço, cheddar, cebola, tomate | n/a |
| Perecíveis | `n` | carne | 8 turnos |
| | `o` | ovo frito | 5 turnos |
| | `b` | bacon | 10 turnos |

Perecíveis que passam da validade são descartados no início do turno. Se você
fechar o hambúrguer com o pão de cima e ele não bater com o pedido, é preciso
remover o topo e consertar.

### Controles

**Terminal:** `p <símbolo>` prepara, `a <símbolo>` adiciona, `r` remove o topo
e `s` sai da partida. Exemplo: `p n` prepara uma carne e `a n` a coloca no
hambúrguer.

**Gráfica:**

| Tecla | Ação | Turno |
|---|---|---|
| letra (`n`, `k`, `z`...) | adicionar ingrediente | 1 |
| `Shift` + letra | preparar ingrediente | 1 |
| `Backspace` | remover o topo (vai para o lixo) | 1 |
| `Esc` | sair da partida (pede confirmação) | n/a |

Com o mouse: clique no pote para adicionar; botão `+` do pote (ou clique
direito) para preparar; botão "Remover topo" ao lado da bandeja.

## Compilar e rodar

Precisa de `gcc` e `make`.

### Versão de terminal (sem dependências)

```sh
make texto
./dado_burger data/pedidos.txt data/historico.txt
```

### Versão gráfica (raylib 5.0)

```sh
make
./dado_burger data/pedidos.txt data/historico.txt
```

O Makefile procura a raylib em `~/raylib-5.0_linux_amd64` (release
pré-compilada para Linux). Se estiver em outro lugar:
`make RAYLIB=/caminho/da/raylib`. Se aparecer `cannot find -lGL`:
`sudo apt install libgl-dev`.

A pasta `assets/` (fontes Fredoka e Nunito, licença OFL) precisa ficar junto do
executável. Sem ela o jogo funciona, mas com a fonte padrão da raylib.

### Compilação manual (terminal)

```sh
gcc -Wall -std=c99 -Iinclude -o dado_burger src/main.c src/avl.c src/hash.c \
    src/heap.c src/pilha.c src/fila.c src/lista.c src/timer.c \
    src/ingredientes.c src/arquivos.c src/pedidos.c src/historico.c \
    src/partida.c src/relatorio.c src/sessao.c src/textos.c \
    src/personagens.c src/interface_texto.c -lm
```

### Arquivos de dados

O programa recebe dois arquivos pela linha de comando: os pedidos e o
histórico. Os que estão em `data/` são os do desenvolvimento (cerca de 837 mil
pedidos e 100 mil entradas de histórico).

- **Pedidos:** duas linhas por pedido, o código e a receita (por exemplo
  `1` e `pbtmcnz`).
- **Histórico:** duas linhas por partida, o nome e a pontuação. Cada partida
  terminada é **acrescentada** ao arquivo informado, preservando as anteriores.

A cada execução o programa cria (ou sobrescreve) o `medicoes.log` com as
métricas de desempenho: carga da AVL (nós, altura, rotações e tempo), carga do
histórico, ordenação, e ao fim de cada partida as buscas na AVL e a tabela
hash (hits, misses, colisões e fator de carga). Os valores são acumulados na
sessão, então o último relatório do arquivo resume a execução inteira.

## Estruturas de dados

| Estrutura | Onde é usada |
|---|---|
| **Árvore AVL** | armazena todos os pedidos (código → receita); busca em O(log n) |
| **Tabela hash** (encadeamento separado) | cache de pedidos já sorteados |
| **Lista encadeada** | receita do cliente da frente, na ordem de montagem |
| **Pilha** | hambúrguer em montagem; a conferência com a receita usa uma pilha auxiliar |
| **Fila** (FIFO) | clientes esperando |
| **Heap mínimo** | um por perecível, ordenado pelo turno de vencimento |
| **Lista contígua** (vetor dinâmico) | histórico de partidas |
| **Merge Sort** | ordena uma cópia do histórico por pontuação ou nome, com o tempo medido |

Com o conjunto de desenvolvimento (837.253 pedidos), a AVL fica com altura 20
e cerca de 19 comparações por busca.

## Arquitetura

Somente os módulos `interface_*` conversam com o jogador. Lógica do jogo,
estruturas e métricas nunca imprimem nem leem o teclado: devolvem códigos de
resultado, e cada interface os transforma em mensagem (`relatorio` escreve em
um `FILE*` recebido, que pode ser a tela ou o log). Por isso as duas
interfaces reaproveitam 100% da lógica.

```
interface_texto / interface_raylib
            │
          sessao ──── pedidos ──── avl, hash
            │   ├──── historico
            │   └──── relatorio (medicoes.log)
          partida ─── fila, lista, pilha, heap
```

### Módulos

| Módulo | Papel |
|---|---|
| `avl` | AVL dos pedidos (carga + buscas, com métricas) |
| `hash` | cache de pedidos, encadeamento separado |
| `pedidos` | AVL + hash juntas: sorteio com consulta ao cache |
| `lista` | lista encadeada da receita |
| `pilha` | montagem do hambúrguer + verificação com a receita |
| `fila` | fila de clientes |
| `heap` | heap mínimo de cada perecível (turno de vencimento) |
| `historico` | lista contígua do histórico + Merge Sort cronometrado |
| `partida` | regras de uma partida (turnos, paciência, validade, fim) |
| `sessao` | carga dos arquivos, `medicoes.log`, fim de partida |
| `relatorio` | métricas no formato do Apêndice B (tela e log) |
| `arquivos` | leitura dos arquivos de entrada |
| `ingredientes` | tabela de ingredientes (símbolo, nome, categoria) |
| `textos` | textos de "Como jogar" e mensagens (comuns às duas interfaces) |
| `timer` | cronômetro |
| `personagens` | nome e aparência dos clientes (novo cliente = uma linha na tabela) |
| `interface_raylib` | interface gráfica |
| `interface_texto` | interface de terminal |

### Estrutura do repositório

```
├── src/          código (.c)
├── include/      cabeçalhos (.h); config.h tem os parâmetros do jogo
├── assets/       fontes da interface gráfica
├── data/         arquivos de pedidos e de histórico
└── Makefile
```

## Decisões de projeto

- **Tamanho da hash (m = 211) e função de hash (divisão):** número primo, longe
  de potências de 2 e de 10, o que dispersa bem as chaves. Dimensionado para
  uma sessão com várias partidas (cerca de 100 entradas mantêm λ ≤ ~0,5).
- **Paciência máxima (16 turnos):** nos pedidos, o mínimo de turnos por receita
  (1 por ingrediente + 1 de preparo para cada um que precisa) vai de 6 a 13,
  com média ≈ 11. Com 16, até a pior receita deixa 3 turnos de folga.
- **Clientes por partida (10):** cerca de 110 ações, uma partida de poucos
  minutos.
- **Validade (carne 8 / ovo 5 / bacon 10):** o ovo exige preparo na hora e o
  bacon permite adiantar para o próximo cliente.
- **AVL:** os pedidos chegam em ordem crescente, que seria o pior caso de uma
  árvore de busca comum. Há uma busca por *teto* (menor código ≥ x) para o
  caso de o código sorteado não existir, resolvida em uma única busca.
- **Ordenação do histórico (Merge Sort):** O(n log n) no pior caso e estável; o
  Quicksort degrada com muitas chaves repetidas (pontuações iguais). A
  ordenação é feita sobre uma cópia, e só o algoritmo é cronometrado.
- **Adaptações nos TADs:** comentadas no topo de cada `.h`.
- **Métricas de busca e de hash:** acumuladas na sessão inteira, porque o cache
  também vive a sessão inteira.
- **Valores configuráveis:** todos em `include/config.h`.

## Autores

Lucas ([@lucasbfonte](https://github.com/lucasbfonte)) e Rodrigo.
