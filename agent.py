from langchain_openai import ChatOpenAI
from langchain.agents import create_tool_calling_agent, AgentExecutor
from langchain_core.prompts import ChatPromptTemplate
from tools import (
    get_outgoing_edges,
    get_incoming_edges,
    get_edge_cost,
    check_vertex_exists,
    check_edge_exists,
    get_all_vertices,
    get_all_edges,
    get_vertex_details,
    get_edge_details,
    get_full_graph_structure,
    get_graph_statistics,
    algo_initialize_state,
    algo_relax_edge,
    algo_get_current_state,
    algo_reconstruct_path,
)


def create_graph_agent(model_name: str, max_iterations: int):
    llm = ChatOpenAI(model=model_name, temperature=0)

    tools = [
        algo_initialize_state,
        algo_relax_edge,
        algo_get_current_state,
        algo_reconstruct_path,
        get_full_graph_structure,
        get_graph_statistics,
        get_outgoing_edges,
        get_incoming_edges,
        get_edge_cost,
        check_vertex_exists,
        check_edge_exists,
        get_all_vertices,
        get_all_edges,
        get_vertex_details,
        get_edge_details,
    ]

    system_message = """
    Você é um ORQUESTRADOR de Algoritmos de Grafos.
    Sua tarefa NÃO é fazer contas de cabeça, mas sim CONTROLAR a memória do algoritmo usando as ferramentas disponíveis.

    ### OBJETIVO:
    Executar o algoritmo de Bellman-Ford (ou Dijkstra) usando a "Memória de Algoritmo" para encontrar o caminho mínimo da origem para TODOS os nós.

    ### FERRAMENTAS DE MEMÓRIA (USE-AS!):
    1. `algo_initialize_state(source)`: Reseta a memória e define dist[source]=0.
    2. `algo_relax_edge(u, v, weight)`: Faz a conta: Se dist[u] + w < dist[v], atualiza dist[v]. Retorna se houve mudança.
    3. `algo_reconstruct_path(target)`: Retorna o caminho final formatado.

    ### ALGORITMO DE EXECUÇÃO (BELLMAN-FORD):
    
    1. **Inicialização**:
       - Chame `get_graph_statistics` para ver o tamanho do grafo (V).
       - Chame `get_full_graph_structure` para ver todas as arestas.
       - Chame `algo_initialize_state(source)`.

    2. **Loop de Relaxamento (Repita V-1 vezes)**:
       - Para CADA iteração:
         - Percorra TODAS as arestas do grafo (que você pegou no passo 1).
         - Para cada aresta (u -> v com custo w), chame `algo_relax_edge(u, v, w)`.
         - Se em uma iteração inteira NENHUMA aresta for atualizada (o retorno das ferramentas disser "No update" para todas), você pode parar o loop mais cedo (otimização).

    3. **Relatório Final**:
       - Para CADA vértice do grafo:
         - Chame `algo_reconstruct_path(vertice_id)`.
         - Colete o resultado.

    ### FORMATO DE SAÍDA:
    Apresente apenas o relatório final com os caminhos e custos retornados pela ferramenta `algo_reconstruct_path`.
    """

    prompt = ChatPromptTemplate.from_messages(
        [
            ("system", system_message),
            ("user", "{input}"),
            ("placeholder", "{agent_scratchpad}"),
        ]
    )

    agent = create_tool_calling_agent(llm, tools, prompt)

    executor = AgentExecutor(
        agent=agent, tools=tools, verbose=True, max_iterations=max_iterations
    )

    return executor
