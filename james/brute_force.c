/*
 * cmsc 142 - minimum graph coloring project
 * author: james
 * 
 * this program finds the chromatic number of a graph using exhaustive
 * search over all possible color assignments.
 * 
 * algorithm:
 *   1. start with k = 1 color
 *   2. generate all k^v possible color assignments (treat as base-k number)
 *   3. validate each assignment by checking all edges
 *   4. if valid coloring found, k is the chromatic number
 *   5. otherwise, increment k and repeat
 *
 * time complexity: o(k^v * e) for each k value tried
 * reference: https://www.geeksforgeeks.org/graph-coloring-applications/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define max_vertices 20 /* maximum number of vertices */
#define max_edges 200 /* maximum number of edges */
#define max_colors_to_try 15 /* maximum chromatic number to try */
#define max_execution_time_seconds 60 /* maximum execution time in seconds */

/* structure to represent a graph */
typedef struct {
    int num_vertices;
    int num_edges;
    int adjacency[max_vertices][max_vertices];
} graph;

/* structure to hold results */
typedef struct {
    int chromatic_number;
    int coloring[max_vertices];
    double time_elapsed;
    bool found;
    bool aborted;
    char abort_reason[256];
} coloringresult;

/*
 * load a graph from a text file.
 * file format:
 *   line 1: v e (number of vertices and edges)
 *   lines 2 to e+1: u v (edge connecting vertex u to vertex v, 0-indexed)
 */
bool load_graph_from_file(const char *filename, graph *g) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        return false;
    }
    
    /* read number of vertices and edges from first line */
    if (fscanf(file, "%d %d", &g->num_vertices, &g->num_edges) != 2) {
        fprintf(stderr, "error: invalid file format. expected 'v e' on first line.\n");
        fclose(file);
        return false;
    }
    
    /* validate graph size against defined limits */
    if (g->num_vertices > max_vertices) {
        fprintf(stderr, "error: too many vertices (%d). maximum supported: %d\n", 
                g->num_vertices, max_vertices);
        fclose(file);
        return false;
    }
    
    if (g->num_edges > max_edges) {
        fprintf(stderr, "error: too many edges (%d). maximum supported: %d\n", 
                g->num_edges, max_edges);
        fclose(file);
        return false;
    }
    
    /* initialize adjacency matrix to all zeros (no edges) */
    for (int i = 0; i < g->num_vertices; i++) {
        for (int j = 0; j < g->num_vertices; j++) {
            g->adjacency[i][j] = 0;
        }
    }
    
    /* read all edges and populate adjacency matrix */
    for (int i = 0; i < g->num_edges; i++) {
        int u, v;
        if (fscanf(file, "%d %d", &u, &v) != 2) {
            fprintf(stderr, "error: could not read edge %d. check file format.\n", i + 1);
            fclose(file);
            return false;
        }
        
        /* validate vertex indices are within bounds */
        if (u < 0 || u >= g->num_vertices || v < 0 || v >= g->num_vertices) {
            fprintf(stderr, "error: edge %d has invalid vertex index (%d, %d). "
                    "valid range: 0 to %d\n", i + 1, u, v, g->num_vertices - 1);
            fclose(file);
            return false;
        }
        
        /* add edge to adjacency matrix (undirected graph: symmetric) */
        g->adjacency[u][v] = 1;
        g->adjacency[v][u] = 1;
    }
    
    fclose(file);
    return true;
}

/*
 * check if a color assignment has any conflicts.
 * a conflict occurs when two adjacent vertices have the same color.
 */
bool has_adjacent_conflict(const graph *g, const int colors[]) {
    /* check every pair of vertices for adjacent conflicts */
    for (int u = 0; u < g->num_vertices; u++) {
        for (int v = u + 1; v < g->num_vertices; v++) {
            /* if there's an edge between u and v */
            if (g->adjacency[u][v] == 1) {
                /* check if they have the same color */
                if (colors[u] == colors[v]) {
                    return true;  /* conflict found */
                }
            }
        }
    }
    return false;  /* no conflicts - valid coloring */
}

/*
 * validate if a color assignment uses only colors from 0 to k-1.
 * this ensures we're checking a proper k-coloring.
 */
bool uses_valid_colors(const int colors[], int num_vertices, int k) {
    for (int i = 0; i < num_vertices; i++) {
        if (colors[i] < 0 || colors[i] >= k) {
            return false;
        }
    }
    return true;
}

/*
 * check if a color assignment is valid for k colors.
 * a valid k-coloring must:
 *   1. use only colors in range [0, k-1]
 *   2. have no adjacent vertices with the same color
 */
bool is_valid_coloring(const graph *g, const int colors[], int k) {
    if (!uses_valid_colors(colors, g->num_vertices, k)) {
        return false;
    }
    return !has_adjacent_conflict(g, colors);
}

/*
 * generate the next color assignment by incrementing a base-k number.
 * we treat the color assignment as a number in base k with v digits.
 * to generate all combinations, we simply increment this number.
 * example for v=3, k=2:
 *   [0,0,0] -> [0,0,1] -> [0,1,0] -> [0,1,1] -> [1,0,0] -> [1,0,1] -> [1,1,0] -> [1,1,1] -> done
 */
bool next_color_assignment(int colors[], int num_vertices, int k) {
    int position = 0;
    
    while (position < num_vertices) {
        /* increment current digit */
        colors[position]++;
        
        /* if no carry needed, we're done */
        if (colors[position] < k) {
            return true;
        }
        
        /* carry to next digit: reset current and continue */
        colors[position] = 0;
        position++;
    }
    
    /* all combinations tried (overflow) */
    return false;
}

/*
 * check if execution time has exceeded the safety threshold.
 */
bool check_timeout(clock_t start_time) {
    clock_t current_time = clock();
    double elapsed = ((double)(current_time - start_time)) / CLOCKS_PER_SEC;
    return elapsed > max_execution_time_seconds;
}

/*
 * find the chromatic number using brute force enumeration.
 * for each k from 1 to max_k:
 *   - generate all k^v color assignments
 *   - check each for validity
 *   - return first valid coloring found
 */
coloringresult find_chromatic_number(const graph *g, int max_k, clock_t start_time) {
    coloringresult result;
    result.found = false;
    result.aborted = false;
    result.chromatic_number = -1;
    
    int colors[max_vertices];
    
    /* try increasing number of colors starting from 1 */
    for (int k = 1; k <= max_k; k++) {
        /* initialize all vertices to color 0 */
        for (int i = 0; i < g->num_vertices; i++) {
            colors[i] = 0;
        }
        
        /* generate and test all k^v color assignments */
        bool more_assignments = true;
        while (more_assignments) {
            /* check timeout to prevent excessive runtime */
            if (check_timeout(start_time)) {
                result.aborted = true;
                snprintf(result.abort_reason, sizeof(result.abort_reason),
                    "execution time exceeded %d seconds", max_execution_time_seconds);
                return result;
            }
            
            /* check if current assignment is valid */
            if (is_valid_coloring(g, colors, k)) {
                /* found a valid k-coloring */
                result.found = true;
                result.chromatic_number = k;
                for (int i = 0; i < g->num_vertices; i++) {
                    /* store colors as 1-indexed for output */
                    result.coloring[i] = colors[i] + 1;
                }
                return result;
            }
            
            /* generate next color assignment */
            more_assignments = next_color_assignment(colors, g->num_vertices, k);
        }
    }
    
    /* exceeded max_k without finding valid coloring */
    result.aborted = true;
    snprintf(result.abort_reason, sizeof(result.abort_reason),
        "exceeded maximum colors to try (%d)", max_k);
    return result;
}

/*
 * print graph information summary.
 */
void print_graph_info(const graph *g) {
    printf("graph statistics:\n");
    printf("  vertices: %d\n", g->num_vertices);
    printf("  edges: %d\n", g->num_edges);
    
    /* calculate and display maximum degree */
    int max_degree = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        int degree = 0;
        for (int j = 0; j < g->num_vertices; j++) {
            degree += g->adjacency[i][j];
        }
        if (degree > max_degree) {
            max_degree = degree;
        }
    }
    printf("  maximum degree: %d\n", max_degree);
    printf("\n");
}

/*
 * print the coloring results.
 */
void print_results(const coloringresult *result, const graph *g) {
    printf("===================================\n");
    printf("graph coloring - brute force search\n");
    printf("===================================\n\n");
    
    print_graph_info(g);
    
    if (result->aborted) {
        printf("*** search aborted ***\n");
        printf("reason: %s\n", result->abort_reason);
        printf("\nthe graph may be too large for brute force approach.\n");
        return;
    }
    
    if (!result->found) {
        printf("*** no solution found ***\n");
        printf("could not find a valid coloring within constraints.\n");
        return;
    }
    
    printf("*** results ***\n");
    printf("chromatic number: %d\n", result->chromatic_number);
    printf("\nvertex coloring:\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("  vertex %d: color %d\n", i, result->coloring[i]);
    }
    
    printf("\ntime elapsed: %.6f seconds\n", result->time_elapsed);
    printf("\nthis solution is optimal (guaranteed minimum).\n");
}


/*
 * print usage instructions.
 */
void print_usage(const char *program_name) {
    printf("usage: %s <graph_file>\n\n", program_name);
    printf("finds the chromatic number of a graph using brute force search.\n\n");
    printf("arguments:\n");
    printf("  graph_file    path to graph input file\n\n");
    printf("file format:\n");
    printf("  v e           (first line: vertices and edges)\n");
    printf("  u1 v1         (edge 1: vertex indices, 0-indexed)\n");
    printf("  ...\n\n");
    printf("example:\n");
    printf("  %s test-cases/small_graph.txt\n", program_name);
}

/*
 * check if graph size is within practical limits.
 */
bool check_size_limits(int num_vertices, int max_k) {
    /* warn if graph is likely too large for brute force */
    if (num_vertices > 12) {
        printf("warning: graph has %d vertices.\n", num_vertices);
        printf("brute force complexity is o(k^v * e).\n");
        printf("this may take a very long time.\n\n");
        return false;
    }
    return true;
}

/*
 * main entry point.
 */
int main(int argc, char *argv[]) {
    /* check command line arguments */
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    /* load graph from file */
    graph g;
    if (!load_graph_from_file(argv[1], &g)) {
        return 1;
    }
    
    /* warn about large graphs */
    check_size_limits(g.num_vertices, max_colors_to_try);
    
    /* record start time for performance measurement */
    clock_t start_time = clock();
    
    /* perform brute force search */
    coloringresult result = find_chromatic_number(&g, max_colors_to_try, start_time);
    
    /* record end time */
    clock_t end_time = clock();
    result.time_elapsed = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    /* print results */
    print_results(&result, &g);
    
    /* return appropriate exit code */
    if (result.found) {
        return 0;
    } else {
        return 2;  /* no solution found */
    }
}
