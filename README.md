# Fluxo em Redes — Projeto C++

Este projeto fornece uma estrutura simples de grafo direcionado em C++ para problemas de fluxo em redes, com separação entre headers (`include/`) e implementações (`src/`), smart pointers e um script de automação para build, execução e plot.

## Estrutura

```
project/
├── include/
│   ├── vertex.hpp
│   ├── edge.hpp
│   ├── graph.hpp
│   └── io.hpp
└── src/
        ├── graph.cpp
        ├── io.cpp
        └── main.cpp

instances/
├── graph_10.txt
└── graph_100.txt

run.sh
```

## Conceitos
- `source`: vértice de origem (fonte) do fluxo.
- `sink`: vértice de destino (sumidouro) do fluxo.
- `capacity`: capacidade máxima de uma aresta.
- `flow`: fluxo atual em uma aresta.
- `residual`: capacidade restante (`capacity - flow`).

## Formato das Instâncias
Cada linha representa uma aresta `u v capacity`:

```
u v capacity
```

Exemplo (`instances/graph_10.txt`):
```
0 1 10
0 2 8
1 3 5
...
8 9 7
```

Os vértices são criados automaticamente conforme aparecem nas arestas.

## Script `run.sh`
O script centraliza build e execução.

Comandos principais:
```
# compilar
./run.sh build

# rodar (compila se necessário)
./run.sh run --input <caminho> --source <id> --sink <id> [--output <caminho>]

# compilar e rodar numa tacada só
./run.sh all --input <caminho> --source <id> --sink <id> [--output <caminho>]

# plotar grafo (gera DOT e PNG)
./run.sh plot --input <caminho> --source <id> --sink <id> [--dot <arquivo.dot>]
```

Exemplos:
```
./run.sh build
./run.sh run --input instances/graph_10.txt --source 0 --sink 9
./run.sh run --input instances/graph_100.txt --source 0 --sink 99 --output result_100.txt

# gerar visualização (DOT + PNG)
./run.sh plot --input instances/graph_10.txt --source 0 --sink 9
```

Observação: no modo `plot`, o script gera um arquivo DOT e, se o Graphviz estiver instalado, renderiza um PNG e abre a imagem automaticamente no macOS.

## Flags suportadas pelo binário
- `--input <caminho>`: arquivo de instância (formato `u v capacity`).
- `--output <caminho>`: arquivo de saída do relatório (opcional).
- `--source <id>`: ID do vértice fonte.
- `--sink <id>`: ID do vértice sumidouro.
- `--dot <caminho.dot>`: exporta o grafo em formato Graphviz DOT.

## Visualização com Graphviz
Instale o Graphviz (macOS):
```
brew install graphviz
```

Instale o Graphviz (linux):
```
apt get install graphviz
```

Renderize DOT para PNG:
```
dot -Tpng graph_10.dot -o graph_10.png
open graph_10.png
```

## Funções principais (código)
- `Graph`
    - `add_vertex(label)`
    - `add_edge(from, to, capacity)`
    - consultas: `vertices()`, `edges()`, `outgoing(v)`, `incoming(v)`
    - fluxo/capacidade: `set_edge_capacity`, `edge_capacity`, `set_edge_flow`, `edge_flow`
- IO (`io.hpp`/`io.cpp`)
    - `Graph load_graph_from_file(const std::string& path)`
    - `FlowReport build_flow_report(const Graph& g)`
    - `void write_report_to_file(const std::string& path, const FlowReport& report)`
    - `void write_dot(const std::string& path, const Graph& g)`

## Requisitos de compilação
Se necessário, torne o script executável:
```
chmod +x run.sh
```
