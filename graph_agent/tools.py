try:
    from . import graph_native
except ImportError:
    import graph_native
from langchain.tools import tool
from typing import List, Dict, Optional
import logging
import os
from datetime import datetime

CURRENT_GRAPH = None
LOGGER = None


def setup_logger():
    global LOGGER
    if LOGGER is not None:
        return LOGGER

    log_dir = os.path.join(os.path.dirname(__file__), "logs")
    os.makedirs(log_dir, exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_file = os.path.join(log_dir, f"algo_execution_{timestamp}.log")

    logger = logging.getLogger("GraphAlgoLogger")
    logger.setLevel(logging.INFO)

    # File handler
    fh = logging.FileHandler(log_file)
    fh.setLevel(logging.INFO)

    # Formatter
    formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
    fh.setFormatter(formatter)

    logger.addHandler(fh)
    LOGGER = logger
    return logger


class AlgorithmState:
    def __init__(self):
        self.distances: Dict[int, float] = {}
        self.predecessors: Dict[int, int] = {}
        self.source: int = -1
        self.initialized: bool = False


ALGO_STATE = AlgorithmState()


def set_current_graph(graph):
    global CURRENT_GRAPH
    CURRENT_GRAPH = graph


@tool
def algo_initialize_state(source_node: int) -> str:
    """
    Initializes the algorithm memory for a new run.
    Sets distance[source] = 0 and all others to Infinity.
    Clears predecessors.
    """
    logger = setup_logger()
    logger.info(f"Initializing algorithm state with source node: {source_node}")

    global ALGO_STATE
    if CURRENT_GRAPH is None:
        logger.error("Attempted to initialize state without a loaded graph.")
        return "Error: Graph not loaded."

    ALGO_STATE = AlgorithmState()
    ALGO_STATE.source = source_node
    ALGO_STATE.initialized = True

    for v in CURRENT_GRAPH.vertices():
        ALGO_STATE.distances[v] = float("inf")

    ALGO_STATE.distances[source_node] = 0.0
    logger.info(
        f"State initialized. Source: {source_node}. All other distances set to INF."
    )
    return f"Memory initialized. Source: {source_node}. Distances set to INF, Source set to 0."


@tool
def algo_relax_edge(u: int, v: int, weight: int) -> str:
    """
    Performs the 'RELAX' operation (Calculation + Comparison + Update).
    Logic:
    1. Reads current dist[u] and dist[v] from memory.
    2. Calculates new_dist = dist[u] + weight.
    3. Compares: If new_dist < dist[v], updates dist[v] and sets predecessor[v] = u.

    Returns a string indicating if an update occurred.
    """
    logger = setup_logger()

    if not ALGO_STATE.initialized:
        logger.error("Attempted to relax edge without initialization.")
        return "Error: Memory not initialized. Call algo_initialize_state first."

    dist_u = ALGO_STATE.distances.get(u, float("inf"))
    dist_v = ALGO_STATE.distances.get(v, float("inf"))

    if dist_u == float("inf"):
        logger.debug(f"Relax skipped: Node {u} is unreachable.")
        return f"No update. Node {u} is currently unreachable (dist=INF)."

    new_dist = dist_u + weight

    if new_dist < dist_v:
        ALGO_STATE.distances[v] = new_dist
        ALGO_STATE.predecessors[v] = u
        logger.info(
            f"RELAX SUCCESS: {u}->{v} (w={weight}). Updated dist[{v}] from {dist_v} to {new_dist}."
        )
        return f"UPDATED: Node {v} distance reduced from {dist_v} to {new_dist}. Predecessor set to {u}."
    else:
        logger.debug(
            f"Relax failed: {u}->{v} (w={weight}). New dist {new_dist} >= current {dist_v}."
        )
        return f"No update. Current path to {v} ({dist_v}) is better or equal to new path ({new_dist})."


@tool
def algo_get_current_state() -> Dict[str, any]:
    """
    Returns the current snapshot of the algorithm memory:
    - Current Distances
    - Current Predecessors
    Use this to check the progress of the algorithm.
    """
    return {
        "distances": {
            k: (str(v) if v == float("inf") else v)
            for k, v in ALGO_STATE.distances.items()
        },
        "predecessors": ALGO_STATE.predecessors,
    }


@tool
def algo_reconstruct_path(target_node: int) -> str:
    """
    Reconstructs the path from Source to Target using the predecessor memory.
    Returns the formatted path string and total cost.
    """
    logger = setup_logger()

    if not ALGO_STATE.initialized:
        logger.error("Attempted to reconstruct path without initialization.")
        return "Error: Memory not initialized."

    if ALGO_STATE.distances.get(target_node) == float("inf"):
        logger.info(f"Path reconstruction failed: Target {target_node} is unreachable.")
        return f"Target {target_node} is unreachable from source {ALGO_STATE.source}."

    path = []
    curr = target_node

    # Safety check for cycles or infinite loops
    visited = set()

    while curr != ALGO_STATE.source:
        if curr in visited:
            logger.error(
                f"Cycle detected during path reconstruction for target {target_node}."
            )
            return "Error: Cycle detected in predecessor path."
        visited.add(curr)

        path.append(curr)
        pred = ALGO_STATE.predecessors.get(curr)
        if pred is None:
            logger.error(f"Broken path: Predecessor for {curr} missing.")
            return f"Error: Broken path. Predecessor for {curr} missing."
        curr = pred

    path.append(ALGO_STATE.source)
    path.reverse()

    path_str = " -> ".join(map(str, path))
    cost = ALGO_STATE.distances[target_node]

    logger.info(
        f"Path reconstructed for target {target_node}: {path_str} (Cost: {cost})"
    )
    return f"Path: {path_str} | Total Cost: {cost}"


@tool
def get_outgoing_edges(vertex_id: int) -> List[int]:
    """Returns a list of edge IDs for edges outgoing from the specified vertex."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.outgoing(vertex_id)


@tool
def get_incoming_edges(vertex_id: int) -> List[int]:
    """Returns a list of edge IDs for edges incoming to the specified vertex."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.incoming(vertex_id)


@tool
def get_edge_cost(edge_id: int) -> int:
    """Returns the cost of the specified edge."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.edge_cost(edge_id)


@tool
def check_vertex_exists(vertex_id: int) -> bool:
    """Checks if a vertex with the given ID exists in the graph."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.has_vertex(vertex_id)


@tool
def check_edge_exists(edge_id: int) -> bool:
    """Checks if an edge with the given ID exists in the graph."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.has_edge(edge_id)


@tool
def get_all_vertices() -> List[int]:
    """Returns a list of all vertex IDs in the graph."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.vertices()


@tool
def get_full_graph_structure() -> Dict[int, List[Dict[str, int]]]:
    """
    Returns the ENTIRE graph structure as a dictionary.
    Keys are vertex IDs. Values are lists of neighbors.
    Each neighbor is a dict with 'to' (destination vertex ID) and 'cost'.
    Use this tool FIRST to get the whole map at once.
    """
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.get_full_graph_structure()


@tool
def get_graph_statistics() -> Dict[str, any]:
    """
    Returns statistics about the graph:
    - num_vertices
    - num_edges
    - has_negative_edges (True/False) - Important for algorithm selection!
    - min_edge_cost
    - max_edge_cost
    """
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.get_graph_statistics()


@tool
def get_all_edges() -> List[int]:
    """Returns a list of all edge IDs in the graph."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    return CURRENT_GRAPH.edges()


@tool
def get_vertex_details(vertex_id: int) -> Dict:
    """Returns details (id, label) for the specified vertex."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    vertex = CURRENT_GRAPH.get_vertex(vertex_id)
    if vertex is None:
        raise ValueError(f"Vertex {vertex_id} not found")
    return {"id": vertex.id, "label": vertex.label}


@tool
def get_edge_details(edge_id: int) -> Dict:
    """Returns details (id, from, to, cost) for the specified edge."""
    if CURRENT_GRAPH is None:
        raise ValueError("Graph not set")
    edge = CURRENT_GRAPH.get_edge(edge_id)
    if edge is None:
        raise ValueError(f"Edge {edge_id} not found")
    return {
        "id": edge.id,
        "from": getattr(edge, "from"),
        "to": edge.to,
        "cost": edge.cost,
    }
