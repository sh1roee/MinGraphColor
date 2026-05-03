// cmsc 142 - minimum graph coloring
// greedy non-brute force implementation

/*

 * time complexity: o(v^2)
 * algorithm:
 *   1. initialize all vertices as unassigned
 *   2. assign the first color to the first vertex
 *   3. for each remaining vertex:
 *      a. find colors used by adjacent vertices and mark them as unavailable
 *      b. find the lowest numbered available color
 *      c. assign this color to the current vertex
 *      d. reset available colors for the next iteration

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define max_vertices 20
#define max_edges 200

typedef struct {
    int v, e;
    int adj[max_vertices][max_vertices];
} graph;

// loads graph from file
bool load_graph(const char *filename, graph *g) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        return false;
    }
    if (fscanf(f, "%d %d", &g->v, &g->e) != 2) {
        fprintf(stderr, "error: invalid file format\n");
        fclose(f);
        return false;
    }
    if (g->v > max_vertices || g->e > max_edges) {
        fprintf(stderr, "error: graph too large\n");
        fclose(f);
        return false;
    }
    for (int i = 0; i < g->v; i++)
        for (int j = 0; j < g->v; j++)
            g->adj[i][j] = 0;
    for (int i = 0; i < g->e; i++) {
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

// greedy coloring algorithm
void greedy_coloring(graph *g) {
    clock_t start = clock();
    
    int colors[max_vertices];
    for (int i = 0; i < g->v; i++) {
        colors[i] = -1; // -1 means unassigned
    }
    
    // assign the first color to first vertex
    colors[0] = 0;
    
    // a temporary array to store the available colors. 
    // available[i] will be true if color i is used by adjacent vertices
    bool available[max_vertices];
    for (int cr = 0; cr < g->v; cr++) {
        available[cr] = false;
    }
    
    // assign colors to remaining V-1 vertices
    for (int u = 1; u < g->v; u++) {
        // process all adjacent vertices and flag their colors as unavailable
        for (int i = 0; i < g->v; i++) {
            if (g->adj[u][i] && colors[i] != -1) {
                available[colors[i]] = true;
            }
        }
        
        // find the first available color
        int cr;
        for (cr = 0; cr < g->v; cr++) {
            if (available[cr] == false) {
                break;
            }
        }
        
        colors[u] = cr; // assign the found color
        
        // reset the values back to false for the next iteration
        for (int i = 0; i < g->v; i++) {
            if (g->adj[u][i] && colors[i] != -1) {
                available[colors[i]] = false;
            }
        }
    }
    
    double time_taken = (double)(clock() - start) / CLOCKS_PER_SEC;
    
    // calculate the number of colors used
    int max_color = 0;
    for (int i = 0; i < g->v; i++) {
        if (colors[i] > max_color) {
            max_color = colors[i];
        }
    }
    
    int num_colors = max_color + 1; // 0-indexed to 1-indexed count
    
    printf("number of colors (greedy approximate): %d\n", num_colors);
    printf("vertex coloring:\n");
    for (int i = 0; i < g->v; i++) {
        printf("  vertex %d: color %d\n", i, colors[i] + 1); // +1 to match 1-indexed output
    }
    printf("time: %.6f seconds\n", time_taken);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <graph_file>\n", argv[0]);
        printf("format: v e followed by e lines of u v (0-indexed)\n");
        return 1;
    }
    
    graph g;
    if (!load_graph(argv[1], &g)) return 1;
    
    greedy_coloring(&g);
    
    return 0;
}
