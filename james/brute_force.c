// cmsc 142 - minimum graph coloring
// brute force implementation
// reference: https://www.geeksforgeeks.org/graph-coloring-applications/

/*

 * time complexity: o(k^v * e) for each k value tried
 * algorithm:
 *   1. start with k = 1 color
 *   2. generate all k^v possible color assignments (treat as base-k number)
 *   3. validate each assignment by checking all edges
 *   4. if valid coloring found, k is the chromatic number
 *   5. otherwise, increment k and repeat

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define max_vertices 20
#define max_edges 200
#define max_colors 15
#define max_time 60

typedef struct {
    int v, e;
    int adj[max_vertices][max_vertices];
} graph;

// loads graph from file
bool load_graph(const char *filename, graph *g) {
    FILE *f = fopen(filename, "r"); // file pointer
    if (!f) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        return false;
    }
    if (fscanf(f, "%d %d", &g->v, &g->e) != 2) { // read vertices and edges
        fprintf(stderr, "error: invalid file format\n");
        fclose(f);
        return false;
    }
    if (g->v > max_vertices || g->e > max_edges) { // check if graph is too large
        fprintf(stderr, "error: graph too large\n");
        fclose(f);
        return false;
    }
    for (int i = 0; i < g->v; i++) // initialize adjacency matrix
        for (int j = 0; j < g->v; j++)
            g->adj[i][j] = 0;
    for (int i = 0; i < g->e; i++) { // read edges
        int u, v;
        if (fscanf(f, "%d %d", &u, &v) != 2 || u < 0 || u >= g->v || v < 0 || v >= g->v) {
            fprintf(stderr, "error: invalid edge %d\n", i + 1);
            fclose(f);
            return false;
        }
        g->adj[u][v] = g->adj[v][u] = 1;
    }
    fclose(f);
    return true;
}

// checks if coloring is valid
bool valid_coloring(graph *g, int colors[], int k) {
    for (int i = 0; i < g->v; i++) // check if colors are valid
        if (colors[i] < 0 || colors[i] >= k)
            return false;
    for (int u = 0; u < g->v; u++) // check if adjacent vertices have different colors
        for (int v = u + 1; v < g->v; v++)
            if (g->adj[u][v] && colors[u] == colors[v])
                return false;
    return true;
}

// generates next k-coloring in lexicographic order
bool next_assignment(int colors[], int n, int k) {
    for (int i = 0; i < n; i++) {
        if (++colors[i] < k) return true; // increment and return true
        colors[i] = 0; // reset and carry
    }
    return false;
}

// finds chromatic number
int find_chromatic(graph *g, int colors[], clock_t start) { 
    for (int k = 1; k <= g->v && k <= max_colors; k++) { // try k colors from 1 to v
        for (int i = 0; i < g->v; i++) colors[i] = 0;
        do { // generate all k-colorings
            if ((double)(clock() - start) / CLOCKS_PER_SEC > max_time) { // check timeout
                fprintf(stderr, "error: timeout\n");
                return -1;
            }
            if (valid_coloring(g, colors, k)) return k; // found valid coloring
        } while (next_assignment(colors, g->v, k)); // generate next k-coloring
    }
    return -1;
}

// main function
int main(int argc, char *argv[]) {
    if (argc != 2) { // check arguments
        printf("usage: %s <graph_file>\n", argv[0]);
        printf("format: v e followed by e lines of u v (0-indexed)\n"); // print usage
        return 1;
    }
    
    graph g;
    if (!load_graph(argv[1], &g)) return 1; // load graph
    
    clock_t start = clock(); // start timer
    int colors[max_vertices];
    int chi = find_chromatic(&g, colors, start); // find chromatic number
    double time = (double)(clock() - start) / CLOCKS_PER_SEC; // calculate time
    
    if (chi < 0) { // timeout or limit reached
        printf("no solution found (timeout or limit reached)\n");
        return 2;
    }
    
    printf("chromatic number: %d\n", chi);
    printf("vertex coloring:\n");
    for (int i = 0; i < g.v; i++)
        printf("  vertex %d: color %d\n", i, colors[i] + 1);
    printf("time: %.6f seconds\n", time);
    return 0;
}
