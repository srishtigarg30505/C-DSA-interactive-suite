#include "graph_traversals.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_kruskal_standard()
{
    int V = 4;
    int E = 5;
    KruskalEdge edges[5] = {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4}
    };

    KruskalEdge* mst_edges = malloc((V - 1) * sizeof(KruskalEdge));
    int mst_edge_count = 0;

    kruskal_mst(edges, V, E, mst_edges, &mst_edge_count);

    // MST should have V-1 = 3 edges
    assert(mst_edge_count == 3);

    // Total weight should be 5 (0-3) + 4 (2-3) + 10 (0-1) = 19
    int total_weight = 0;
    for (int i = 0; i < mst_edge_count; i++)
    {
        total_weight += mst_edges[i].weight;
    }
    assert(total_weight == 19);

    free(mst_edges);
    printf("Kruskal standard graph test passed\n");
}

void test_kruskal_disconnected()
{
    int V = 4;
    int E = 2;
    // 0-1 and 2-3 are connected, but the graph is disconnected overall
    KruskalEdge edges[2] = {
        {0, 1, 5},
        {2, 3, 7}
    };

    KruskalEdge* mst_edges = malloc((V - 1) * sizeof(KruskalEdge));
    int mst_edge_count = 0;

    kruskal_mst(edges, V, E, mst_edges, &mst_edge_count);

    // Should only select 2 edges
    assert(mst_edge_count == 2);

    int total_weight = 0;
    for (int i = 0; i < mst_edge_count; i++)
    {
        total_weight += mst_edges[i].weight;
    }
    assert(total_weight == 12);

    free(mst_edges);
    printf("Kruskal disconnected graph test passed\n");
}

void test_kruskal_single_vertex()
{
    int V = 1;
    int E = 0;
    KruskalEdge edges[1] = { {0, 0, 0} }; // dummy edge

    KruskalEdge* mst_edges = malloc(1 * sizeof(KruskalEdge));
    int mst_edge_count = 0;

    kruskal_mst(edges, V, E, mst_edges, &mst_edge_count);

    assert(mst_edge_count == 0);

    free(mst_edges);
    printf("Kruskal single vertex graph test passed\n");
}

void test_kruskal_zero_edges()
{
    int V = 4;
    int E = 0;
    KruskalEdge edges[1] = { {0, 0, 0} }; // dummy edge

    KruskalEdge* mst_edges = malloc((V - 1) * sizeof(KruskalEdge));
    int mst_edge_count = 0;

    kruskal_mst(edges, V, E, mst_edges, &mst_edge_count);

    assert(mst_edge_count == 0);

    free(mst_edges);
    printf("Kruskal zero edges test passed\n");
}

void test_kruskal_negative_weights()
{
    int V = 4;
    int E = 5;
    KruskalEdge edges[5] = {
        {0, 1, -10},
        {0, 2, -6},
        {0, 3, -5},
        {1, 3, -15},
        {2, 3, -4}
    };

    KruskalEdge* mst_edges = malloc((V - 1) * sizeof(KruskalEdge));
    int mst_edge_count = 0;

    kruskal_mst(edges, V, E, mst_edges, &mst_edge_count);

    assert(mst_edge_count == 3);

    // Total weight should be -15 (1-3) + -10 (0-1) + -6 (0-2) = -31
    int total_weight = 0;
    for (int i = 0; i < mst_edge_count; i++)
    {
        total_weight += mst_edges[i].weight;
    }
    assert(total_weight == -31);

    free(mst_edges);
    printf("Kruskal negative weights test passed\n");
}

int main()
{
    test_kruskal_standard();
    test_kruskal_disconnected();
    test_kruskal_single_vertex();
    test_kruskal_zero_edges();
    test_kruskal_negative_weights();

    printf("All Kruskal tests passed\n");
    return 0;
}
