# Dado Burger (INF01203)

## Compilar e rodar

    make
    ./dado_burger pedidos.txt historico.txt

## Decisoes de projeto (preencher conforme formos decidindo)

Estas notas viram materia-prima do relatorio.

- Tamanho da hash (m) e funcao de hash: _por que?_
- Paciencia maxima do cliente: _por que?_
- Numero de clientes por partida: _por que?_
- Validade dos pereciveis (carne / ovo / bacon): _por que?_
- Algoritmo de ordenacao do historico: _por que?_
- Adaptacoes feitas nos TADs das aulas: _o que mudou e por que?_

## Divisao de tarefas

- Lucas:
- Rodrigo:

## Regra de arquitetura (pensando na raylib)

Somente o modulo de interface usa printf/scanf. Logica do jogo, estruturas e
metricas nunca imprimem nem leem teclado.
