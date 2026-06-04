#include "graph_traversals.h"
#include "safe_input.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Disjoint Set Union (DSU) / Union-Find Subset Structure.
 * This structure helps keep track of connected components in a graph
 * to efficiently check for cycles when adding new edges.
 */
typedef struct DSUSubset
{
    int parent; // Parent index of the node
    int rank;   // Height/Rank of the tree (used to keep the tree balanced)
} DSUSubset;

/**
 * Find operation of DSU with Path Compression.
 * Recursively finds the representative/root of the subset containing 'i'.
 * Path compression flattens the tree structure by making every node
 * in the path point directly to the root, optimizing future operations.
 */
static int dsu_find(DSUSubset subsets[], int i)
{
    // If 'i' is not its own parent, then it is not the root of its set.
    if (subsets[i].parent != i)
    {
        // Path compression: update parent to point directly to the root
        subsets[i].parent = dsu_find(subsets, subsets[i].parent);
    }
    return subsets[i].parent;
}

/**
 * Union operation of DSU using Union by Rank.
 * Merges the subsets containing 'x' and 'y'.
 * It attaches the smaller tree (lower rank) under the root of the larger tree
 * to prevent the tree from becoming deep, keeping search operations fast.
 */
static void dsu_union(DSUSubset subsets[], int x, int y)
{
    int xroot = dsu_find(subsets, x);
    int yroot = dsu_find(subsets, y);

    // Attach smaller rank tree under root of high rank tree
    if (subsets[xroot].rank < subsets[yroot].rank)
    {
        subsets[xroot].parent = yroot;
    }
    else if (subsets[xroot].rank > subsets[yroot].rank)
    {
        subsets[yroot].parent = xroot;
    }
    else
    {
        // If ranks are same, make one as root and increment its rank by one
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

/**
 * Comparator function for qsort.
 * Sorts edges in ascending (non-decreasing) order of their weights.
 */
static int compare_edges(const void* a, const void* b)
{
    KruskalEdge* a_edge = (KruskalEdge*)a;
    KruskalEdge* b_edge = (KruskalEdge*)b;
    if (a_edge->weight < b_edge->weight) return -1;
    if (a_edge->weight > b_edge->weight) return 1;
    return 0;
}

/**
 * Kruskal's Minimum Spanning Tree (MST) Algorithm.
 * 
 * @param edges Array containing all edges in the graph
 * @param V Number of vertices
 * @param E Number of edges
 * @param mst_edges Pre-allocated array to store the selected MST edges
 * @param mst_edge_count Pointer to return the number of edges added to the MST
 */
void kruskal_mst(KruskalEdge* edges, int V, int E, KruskalEdge* mst_edges, int* mst_edge_count)
{
    *mst_edge_count = 0;

    // STEP 1: Sort all edges in non-decreasing order of their weights.
    // This allows us to greedily pick the lightest edges first.
    qsort(edges, E, sizeof(KruskalEdge), compare_edges);

    // Allocate memory for subsets to manage cycles
    DSUSubset* subsets = malloc(V * sizeof(DSUSubset));
    if (!subsets)
    {
        printf("\nMemory allocation failed in DSU initialization.\n");
        return;
    }

    // Initialize each vertex as a separate subset containing only itself
    for (int v = 0; v < V; v++)
    {
        subsets[v].parent = v;
        subsets[v].rank = 0;
    }

    int e_index = 0; // Index to iterate through sorted edges
    int m_index = 0; // Index of the next edge to add to the MST

    // An MST must contain exactly (V - 1) edges.
    while (m_index < V - 1 && e_index < E)
    {
        // STEP 2: Pick the lightest remaining edge
        KruskalEdge next_edge = edges[e_index++];

        int x = dsu_find(subsets, next_edge.src);
        int y = dsu_find(subsets, next_edge.dest);

        // STEP 3: Check if adding this edge creates a cycle.
        // If x == y, both vertices are already in the same connected component,
        // so adding this edge would create a cycle. We discard it.
        if (x != y)
        {
            mst_edges[m_index++] = next_edge; // Include the edge in the MST
            dsu_union(subsets, x, y);         // Merge the subsets
        }
    }

    *mst_edge_count = m_index;
    free(subsets);
}

/**
 * Interactive demo for testing Kruskal's MST algorithm.
 */
void kruskal_demo(void)
{
    int vertices = 0;
    int edges_num = 0;

    // 1. Prompt for and validate the number of vertices
    while (1)
    {
        int status = safe_input_int(&vertices,
                                    "\nEnter the number of vertices in the graph (between 1 and 10), or enter '-1' to exit: ",
                                    1, 10);
        if (status == INPUT_EXIT_SIGNAL)
        {
            printf("\nExiting Kruskal's demo...\n");
            return;
        }
        if (status == 0)
        {
            continue;
        }
        break;
    }

    // 2. Prompt for and validate the number of edges
    while (1)
    {
        int max_edges = vertices * (vertices - 1) / 2; // Maximum edges in simple undirected graph
        if (max_edges < 0) max_edges = 0;
        
        int status = safe_input_int(&edges_num,
                                    "\nEnter the number of edges, or enter '-1' to exit: ",
                                    0, max_edges);
        if (status == INPUT_EXIT_SIGNAL)
        {
            printf("\nExiting Kruskal's demo...\n");
            return;
        }
        if (status == 0)
        {
            continue;
        }
        break;
    }

    // Allocate memory (at least 1 element to avoid platform-dependent malloc(0) behavior)
    int alloc_edges = (edges_num > 0) ? edges_num : 1;
    int alloc_mst = (vertices > 1) ? (vertices - 1) : 1;
    KruskalEdge* edges = malloc(alloc_edges * sizeof(KruskalEdge));
    KruskalEdge* mst_edges = malloc(alloc_mst * sizeof(KruskalEdge));

    if (!edges || !mst_edges)
    {
        printf("\nMemory allocation failed.\n");
        if (edges) free(edges);
        if (mst_edges) free(mst_edges);
        return;
    }

    printf("\nEnter source, destination, and weight for each undirected edge:\n");
    printf("(Vertices must be between 0 and %d. Weights can be negative)\n", vertices - 1);

    // 3. Collect edge inputs from the user
    for (int i = 0; i < edges_num; i++)
    {
        int src = 0, dest = 0, weight = 0;
        int status;

    retry_src:
        status = safe_input_int(&src, "Source node: ", 0, vertices - 1);
        if (status == INPUT_EXIT_SIGNAL)
        {
            goto cleanup;
        }
        if (status == 0)
        {
            goto retry_src;
        }

    retry_dest:
        status = safe_input_int(&dest, "Destination node: ", 0, vertices - 1);
        if (status == INPUT_EXIT_SIGNAL)
        {
            goto cleanup;
        }
        if (status == 0)
        {
            goto retry_dest;
        }

    retry_weight:
        status = safe_input_int(&weight, "Edge weight (e.g. -1000 to 1000): ", -1000, 1000);
        if (status == INPUT_EXIT_SIGNAL)
        {
            goto cleanup;
        }
        if (status == 0)
        {
            goto retry_weight;
        }

        edges[i].src = src;
        edges[i].dest = dest;
        edges[i].weight = weight;
    }

    // 4. Run Kruskal's algorithm
    int mst_edge_count = 0;
    kruskal_mst(edges, vertices, edges_num, mst_edges, &mst_edge_count);

    // 5. Display the results
    printf("\nFollowing are the edges in the constructed MST:\n");
    printf("Source  <->  Destination   \t   Weight\n");
    printf("------       -----------   \t   ------\n");

    int total_weight = 0;
    for (int i = 0; i < mst_edge_count; i++)
    {
        printf("  %d    <->       %d       \t     %d\n", mst_edges[i].src, mst_edges[i].dest, mst_edges[i].weight);
        total_weight += mst_edges[i].weight;
    }

    printf("\nTotal weight of Minimum Spanning Tree: %d\n", total_weight);

    // If we have fewer than V-1 edges, it means the graph was disconnected.
    if (mst_edge_count < vertices - 1)
    {
        printf("\nWarning: Graph is disconnected. Generated a Minimum Spanning Forest containing %d connected components.\n",
               vertices - mst_edge_count);
    }

cleanup:
    free(edges);
    free(mst_edges);
}
