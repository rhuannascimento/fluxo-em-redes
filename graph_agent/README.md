# Graph Agent Module

Este módulo implementa um agente de Inteligência Artificial Generativa capaz de raciocinar sobre grafos e executar algoritmos de caminho mínimo (como Bellman-Ford) utilizando uma arquitetura híbrida de "Memória Algorítmica".

## Estrutura dos Arquivos

* **`agent.py`**: Define o "cérebro" do agente. Configura o modelo OpenAI (via LangChain), define o prompt do sistema (que instrui o agente a atuar como um orquestrador de algoritmos e não uma calculadora) e inicializa o `AgentExecutor`.
* **`tools.py`**: Contém as ferramentas (Tools) que o agente utiliza para interagir com o mundo.
    * **Ferramentas de Grafo**: Wrappers para as funções C++ (ex: `get_outgoing_edges`, `get_edge_cost`).
    * **Ferramentas de Estado**: Implementam a memória do algoritmo em Python (dicionários de distâncias e predecessores). O agente usa `algo_relax_edge` para atualizar o estado de forma determinística, evitando alucinações matemáticas.
* **`graph_native.so`** (ou `.pyd`): A extensão compilada em C++ (via Pybind11). É o núcleo de alta performance que carrega o grafo na memória e fornece acesso rápido à estrutura de dados.
* **`__init__.py`**: Marca este diretório como um pacote Python e expõe os módulos necessários.

## Como Preparar

### 1. Pré-requisitos
* Python 3.14+
* Compilador C++ (GCC/Clang/MSVC) com suporte a C++17
* CMake
* Chave da API da OpenAI (definida no arquivo `.env` na raiz do projeto)

### 2. Instalação e Compilação

1. **Ambiente Virtual**:
   Certifique-se de estar no ambiente virtual do projeto:
   ```bash
   source ../.venv/bin/activate
   ```

2. **Dependências**:
   Instale as bibliotecas necessárias:
   ```bash
   pip install -r ../requirements.txt
   ```

3. **Compilação do Módulo Nativo (Build)**:
   O núcleo do agente utiliza uma extensão C++ (`graph_native`) para alta performance. Siga os passos abaixo para compilar:

   * **Gere os arquivos de build com CMake**:
     ```bash
     cmake -S . -B build
     ```

   * **Compile o projeto**:
     ```bash
     cmake --build build
     ```

   * **Instale o módulo (Mover para o diretório correto)**:
     O processo de build irá gerar um arquivo com extensão `.so` (Linux/Mac) ou `.pyd` (Windows) na raiz do projeto. Mova este arquivo para dentro da pasta `graph_agent/`:
     ```bash
     mv graph_native*.so graph_agent/
     # Ou no Windows: move graph_native*.pyd graph_agent/
     ```

## Como Rodar

O agente é projetado para ser executado através do script principal `main.py` localizado na raiz do projeto, que serve como ponto de entrada (CLI).

**Comando Básico:**

```bash
# A partir da raiz do projeto (../)
python main.py <caminho_do_grafo> --source <no_origem>
```

**Exemplo:**

```bash
python main.py instances/dense_dag_10.txt --source 0
```

**Argumentos Opcionais:**
* `--model`: Especifica o modelo OpenAI (ex: `gpt-4o`, `gpt-3.5-turbo`).
* `--iterations`: Limite máximo de passos do agente (padrão: 50).

## Arquitetura

O agente funciona no padrão **ReAct** (Reasoning + Acting), mas com uma camada de memória externa:
1. O Agente **observa** o grafo usando ferramentas C++.
2. O Agente **decide** qual aresta relaxar com base na lógica do Bellman-Ford.
3. O Agente **executa** a ação `algo_relax_edge`.
4. O Python **calcula** o novo custo e atualiza a memória (sem intervenção da LLM na matemática).
5. O ciclo se repete até a convergência.
