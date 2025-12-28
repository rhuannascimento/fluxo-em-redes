import os
import sys
import argparse
from dotenv import load_dotenv
from graph_agent import graph_native
from graph_agent import tools
from graph_agent.agent import create_graph_agent


def main():
    load_dotenv()

    parser = argparse.ArgumentParser(description="Graph Pathfinding Agent")
    parser.add_argument(
        "--input_file",
        default=os.getenv("DEFAULT_GRAPH"),
        help="Path to the graph input file",
    )
    parser.add_argument(
        "--source", type=int, default=0, help="Source node for pathfinding (default: 0)"
    )
    parser.add_argument(
        "--model",
        default=os.getenv("OPENAI_MODEL", "gpt-4o"),
        help="OpenAI model to use",
    )
    parser.add_argument(
        "--iterations",
        type=int,
        default=int(os.getenv("MAX_ITERATIONS", 50)),
        help="Max iterations for the agent",
    )

    args = parser.parse_args()

    # 3. Check if input file exists
    if not os.path.exists(args.input_file):
        print(f"Error: Input file '{args.input_file}' does not exist.")
        sys.exit(1)

    try:
        # 4. Load the graph
        print(f"Loading graph from {args.input_file}...")
        graph = graph_native.load_graph_from_file(args.input_file)

        # 5. Set the global graph
        tools.set_current_graph(graph)

        # 6. Print graph stats
        num_vertices = len(graph.vertices())
        num_edges = len(graph.edges())
        print(f"Graph loaded successfully.")
        print(f"Number of vertices: {num_vertices}")
        print(f"Number of edges: {num_edges}")

        # 7. Initialize agent
        print(f"Initializing agent with model {args.model}...")
        agent_executor = create_graph_agent(
            model_name=args.model, max_iterations=args.iterations
        )

        # 8. Run agent
        query = f"Find the shortest path from node {args.source} to all other nodes. List the costs."
        print(f"Running query: {query}")
        result = agent_executor.invoke({"input": query})

        # 9. Print the result
        print("\n--- Agent Result ---")
        print(result["output"])

    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
