# Dado Burger (INF01203)

## Compilar e rodar

Versao grafica (raylib 5.0, extra da secao 8):

    make
    ./dado_burger pedidos.txt historico.txt

O Makefile procura a raylib em `~/raylib-5.0_linux_amd64` (release pre-compilada).
Se estiver em outro lugar: `make RAYLIB=/caminho/da/raylib`.
Se der `cannot find -lGL`: `sudo apt install libgl-dev`.

Versao terminal (sem raylib, mesma logica):

    make texto
    ./dado_burger pedidos.txt historico.txt

Comando equivalente ao do Apendice C (versao terminal):

    gcc -o dado_burger main.c avl.c hash.c heap.c pilha.c fila.c lista.c timer.c \
        ingredientes.c arquivos.c pedidos.c historico.c partida.c relatorio.c \
        sessao.c textos.c personagens.c interface_texto.c -lm -Wall -std=c99

## Controles (versao grafica)

| Tecla              | Acao                              | Turno |
|--------------------|-----------------------------------|-------|
| letra (n, k, z...) | adicionar ingrediente             | 1     |
| Shift + letra      | preparar ingrediente              | 1     |
| Backspace          | remover o topo (vai para o lixo)  | 1     |
| Esc                | sair da partida (pede confirmacao)| -     |

Mouse: clique no pote = adicionar; botao "+" do pote (ou clique direito) = preparar;
botao "Remover topo" ao lado da bandeja.

A pasta `assets/` (fontes Fredoka e Nunito, licenca OFL) precisa ficar junto do
executavel. Sem ela o jogo funciona, mas com a fonte padrao da raylib.

## Clientes

Os clientes do antonio's ficam na tabela de `personagens.c` (nome, pele, cabelo,
estilo, barba, oculos, roupa). Para adicionar alguem, basta uma linha nova.

## Modulos

| Arquivo            | Papel                                                        |
|--------------------|--------------------------------------------------------------|
| `avl`              | AVL dos pedidos (carga + buscas, com metricas)               |
| `hash`             | cache de pedidos, encadeamento separado                      |
| `pedidos`          | AVL + hash juntas: sorteio com consulta ao cache             |
| `lista`            | lista encadeada da receita                                   |
| `pilha`            | montagem do hamburguer + verificacao com a receita           |
| `fila`             | fila de clientes                                             |
| `heap`             | heap minimo de cada perecivel (turno de vencimento)          |
| `historico`        | lista contigua do historico + Merge Sort cronometrado        |
| `partida`          | regras de uma partida (turnos, paciencia, validade, fim)     |
| `sessao`           | carga dos arquivos, medicoes.log, fim de partida             |
| `relatorio`        | metricas no formato do Apendice B (tela e log)               |
| `arquivos`         | parsing dos arquivos de entrada                              |
| `ingredientes`     | Tabela 1 (simbolo, nome, categoria)                          |
| `textos`           | textos de "Como jogar" e mensagens (comuns as 2 interfaces)  |
| `interface_raylib` | interface grafica                                            |
| `interface_texto`  | interface de terminal                                        |
| `personagens`      | nome e aparencia dos clientes                                |

## Decisoes de projeto (materia-prima do relatorio)

- **Tamanho da hash (m = 211) e funcao de hash (divisao):** primo longe de
  potencias de 2 e 10; ver `config.h`.
- **Paciencia maxima (16 turnos):** no pedidos.txt o minimo de turnos por receita
  vai de 6 a 13 (media ~11). 16 deixa folga de 3 turnos ate na pior receita.
- **Clientes por partida (10):** ~110 acoes, partida de poucos minutos.
- **Validade (carne 8 / ovo 5 / bacon 10):** ovo exige preparo na hora; bacon
  permite adiantar para o proximo cliente.
- **Ordenacao do historico (Merge Sort):** O(n log n) no pior caso e estavel;
  o Quicksort degrada com muitas chaves repetidas (pontuacoes iguais).
- **Adaptacoes nos TADs:** comentadas no topo de cada `.h`.
- **Metricas de busca/hash:** acumuladas na sessao inteira (o cache tambem
  vive a sessao inteira).

## Regra de arquitetura

Somente os modulos `interface_*` conversam com o usuario. Logica do jogo,
estruturas e metricas nunca imprimem nem leem teclado (`relatorio` escreve
em um `FILE*` recebido, que pode ser a tela ou o log).

## Divisao de tarefas

- Lucas:
- Rodrigo:
