// cmsc 142 - minimum graph coloring
// combined brute force and non-brute force (greedy) implementation

/*

 * BRUTE FORCE
 * time complexity: o(k^v * e) for each k value tried
 * algorithm:
 *   1. start with k = 1 color
 *   2. generate all k^v possible color assignments
 *   3. validate each assignment by checking all edges
 *   4. if valid coloring found, k is the chromatic number
 *   5. otherwise, increment k and repeat
 *
 * NON-BRUTE FORCE (GREEDY)
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
#include <string.h>
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

// BRUTE FORCE IMPLEMENTATION

// checks if coloring is valid
bool valid_coloring(graph *g, int colors[], int k) {
    for (int i = 0; i < g->v; i++) // check if colors are valid
        if (colors[i] < 0 || colors[i] >= k)
            return false;
    for (int u = 0; u < g->v; u++) // check if adjacent vertices have different colors
        for (int v = u + 1; v < g->v; v++)
            if (g->adj[u][v] && colors[u] == colors[v]) // if adjacent vertices have same color
                return false;
    return true;
}

// generates next k-coloring in order
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

// solves graph coloring using brute force
void solve_brute_force(graph *g) {
    clock_t start = clock(); // start timer
    int colors[max_vertices];
    int chi = find_chromatic(g, colors, start); // find chromatic number
    double time_taken = (double)(clock() - start) / CLOCKS_PER_SEC; // calculate time
    
    if (chi < 0) { // timeout or limit reached
        printf("no solution found (timeout or limit reached)\n");
        return;
    }
    
    printf("\n--- Brute Force Results ---\n");
    printf("chromatic number: %d\n", chi);
    printf("vertex coloring:\n");
    for (int i = 0; i < g->v; i++)
        printf("  vertex %d: color %d\n", i, colors[i] + 1);
    printf("\ntime: %.6f seconds\n", time_taken);
    printf("---------------------------\n");
}

// NON-BRUTE FORCE (GREEDY) IMPLEMENTATION

void solve_greedy(graph *g) {
    clock_t start = clock();
    
    int colors[max_vertices];
    for (int i = 0; i < g->v; i++) {
        // -1 means unassigned
        colors[i] = -1; 
    }
    
    // assign the first color to first vertex
    colors[0] = 0;
    
    // a temporary array to store the available colors
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
        
        // assign the found color
        colors[u] = cr; 
        
        // reset the values back to false for the next iteration
        for (int i = 0; i < g->v; i++) {
            if (g->adj[u][i] && colors[i] != -1) {
                available[colors[i]] = false;
            }
        }
    }
    
    // record running time
    double time_taken = (double)(clock() - start) / CLOCKS_PER_SEC;
    
    // calculate the number of colors used
    int max_color = 0;
    for (int i = 0; i < g->v; i++) {
        if (colors[i] > max_color) {
            max_color = colors[i];
        }
    }
    
    // 0-indexed to 1-indexed count
    int num_colors = max_color + 1; 
    
    printf("\n--- Non-Brute Force (Greedy) Results ---\n");
    printf("number of colors (greedy approximate): %d\n", num_colors);
    printf("vertex coloring:\n");
    for (int i = 0; i < g->v; i++) {
        printf("  vertex %d: color %d\n", i, colors[i] + 1); // +1 to match 1-indexed output
    }
    printf("\ntime: %.6f seconds\n", time_taken);
    printf("----------------------------------------\n");
}

int main(int argc, char *argv[]) {
    char filename[256];
    
    // If graph file is not provided in command line, prompt user for it
    if (argc >= 2) {
        strncpy(filename, argv[1], sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
    } else {
        printf("Enter graph filename: ");
        if (scanf("%255s", filename) != 1) {
            return 1;
        }
    }
    
    graph g;
    if (!load_graph(filename, &g)) {
        return 1;
    }
    
    int choice;
    while (1) {
        printf("\nSelect Algorithm:\n");
        printf("[1] Solve in Brute Force\n");
        printf("[2] Solve in Non-Brute Force\n");
        printf("[3] Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            // handle non-integer input
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Invalid input. Please try again.\n");
            continue;
        }
        
        if (choice == 1) {
            solve_brute_force(&g);
        } else if (choice == 2) {
            solve_greedy(&g);
        } else if (choice == 3) {
            printf("Goodbye!\n");
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}
