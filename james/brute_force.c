/**
 * brute_force.c - Brute Force Graph Coloring Implementation
 * 
 * CMSC 142 - Minimum Graph Coloring Project
 * Author: James
 * 
 * This program finds the chromatic number of a graph using exhaustive
 * search over all possible color assignments.
 * 
 * Algorithm:
 *   1. Start with k = 1 color
 *   2. Generate all k^V possible color assignments (treat as base-k number)
 *   3. Validate each assignment by checking all edges
 *   4. If valid coloring found, k is the chromatic number
 *   5. Otherwise, increment k and repeat
 * 
 * Time Complexity: O(k^V × E) for each k value tried
 * 
 * Reference: https://www.geeksforgeeks.org/graph-coloring-applications/
 */

#include "brute_force.h"

/**
 * Load a graph from a text file.
 * 
 * The file format is:
 *   Line 1: V E (number of vertices and edges)
 *   Lines 2 to E+1: u v (edge connecting vertex u to vertex v, 0-indexed)
 */
bool load_graph_from_file(const char *filename, Graph *graph) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return false;
    }
    
    /* Read number of vertices and edges from first line */
    if (fscanf(file, "%d %d", &graph->num_vertices, &graph->num_edges) != 2) {
        fprintf(stderr, "Error: Invalid file format. Expected 'V E' on first line.\n");
        fclose(file);
        return false;
    }
    
    /* Validate graph size against defined limits */
    if (graph->num_vertices > MAX_VERTICES) {
        fprintf(stderr, "Error: Too many vertices (%d). Maximum supported: %d\n", 
                graph->num_vertices, MAX_VERTICES);
        fclose(file);
        return false;
    }
    
    if (graph->num_edges > MAX_EDGES) {
        fprintf(stderr, "Error: Too many edges (%d). Maximum supported: %d\n", 
                graph->num_edges, MAX_EDGES);
        fclose(file);
        return false;
    }
    
    /* Initialize adjacency matrix to all zeros (no edges) */
    for (int i = 0; i < graph->num_vertices; i++) {
        for (int j = 0; j < graph->num_vertices; j++) {
            graph->adjacency[i][j] = 0;
        }
    }
    
    /* Read all edges and populate adjacency matrix */
    for (int i = 0; i < graph->num_edges; i++) {
        int u, v;
        if (fscanf(file, "%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Could not read edge %d. Check file format.\n", i + 1);
            fclose(file);
            return false;
        }
        
        /* Validate vertex indices are within bounds */
        if (u < 0 || u >= graph->num_vertices || v < 0 || v >= graph->num_vertices) {
            fprintf(stderr, "Error: Edge %d has invalid vertex index (%d, %d). "
                    "Valid range: 0 to %d\n", i + 1, u, v, graph->num_vertices - 1);
            fclose(file);
            return false;
        }
        
        /* Add edge to adjacency matrix (undirected graph: symmetric) */
        graph->adjacency[u][v] = 1;
        graph->adjacency[v][u] = 1;
    }
    
    fclose(file);
    return true;
}

/**
 * Check if a color assignment has any conflicts.
 * 
 * A conflict occurs when two adjacent vertices have the same color.
 * This function scans all edges to verify the coloring is proper.
 */
bool has_adjacent_conflict(const Graph *graph, const int colors[]) {
    /* Check every pair of vertices for adjacent conflicts */
    for (int u = 0; u < graph->num_vertices; u++) {
        for (int v = u + 1; v < graph->num_vertices; v++) {
            /* If there's an edge between u and v */
            if (graph->adjacency[u][v] == 1) {
                /* Check if they have the same color */
                if (colors[u] == colors[v]) {
                    return true;  /* Conflict found */
                }
            }
        }
    }
    return false;  /* No conflicts - valid coloring */
}

/**
 * Validate if a color assignment uses only colors from 0 to k-1.
 * 
 * This ensures we're checking a proper k-coloring, not just any coloring.
 */
bool uses_valid_colors(const int colors[], int num_vertices, int k) {
    for (int i = 0; i < num_vertices; i++) {
        if (colors[i] < 0 || colors[i] >= k) {
            return false;
        }
    }
    return true;
}

/**
 * Check if a color assignment is valid for k colors.
 * 
 * A valid k-coloring must:
 *   1. Use only colors in range [0, k-1]
 *   2. Have no adjacent vertices with the same color
 */
bool is_valid_coloring(const Graph *graph, const int colors[], int k) {
    if (!uses_valid_colors(colors, graph->num_vertices, k)) {
        return false;
    }
    return !has_adjacent_conflict(graph, colors);
}

/**
 * Generate the next color assignment by incrementing a base-k number.
 * 
 * We treat the color assignment as a number in base k with V digits.
 * To generate all combinations, we simply increment this number.
 * 
 * Example for V=3, k=2:
 *   [0,0,0] -> [0,0,1] -> [0,1,0] -> [0,1,1] -> [1,0,0] -> [1,0,1] -> [1,1,0] -> [1,1,1] -> done
 */
bool next_color_assignment(int colors[], int num_vertices, int k) {
    int position = 0;
    
    while (position < num_vertices) {
        /* Increment current digit */
        colors[position]++;
        
        /* If no carry needed, we're done */
        if (colors[position] < k) {
            return true;
        }
        
        /* Carry to next digit: reset current and continue */
        colors[position] = 0;
        position++;
    }
    
    /* All combinations tried (overflow) */
    return false;
}

/**
 * Check if execution time has exceeded the safety threshold.
 */
bool check_timeout(clock_t start_time) {
    clock_t current_time = clock();
    double elapsed = ((double)(current_time - start_time)) / CLOCKS_PER_SEC;
    return elapsed > MAX_EXECUTION_TIME_SECONDS;
}

/**
 * Find the chromatic number using brute force enumeration.
 * 
 * For each k from 1 to max_k:
 *   - Generate all k^V color assignments
 *   - Check each for validity
 *   - Return first valid coloring found
 */
ColoringResult find_chromatic_number(const Graph *graph, int max_k, clock_t start_time) {
    ColoringResult result;
    result.found = false;
    result.aborted = false;
    result.chromatic_number = -1;
    
    int colors[MAX_VERTICES];
    
    /* Try increasing number of colors starting from 1 */
    for (int k = 1; k <= max_k; k++) {
        /* Initialize all vertices to color 0 */
        for (int i = 0; i < graph->num_vertices; i++) {
            colors[i] = 0;
        }
        
        /* Generate and test all k^V color assignments */
        bool more_assignments = true;
        while (more_assignments) {
            /* Check timeout to prevent excessive runtime */
            if (check_timeout(start_time)) {
                result.aborted = true;
                snprintf(result.abort_reason, sizeof(result.abort_reason),
                    "Execution time exceeded %d seconds", MAX_EXECUTION_TIME_SECONDS);
                return result;
            }
            
            /* Check if current assignment is valid */
            if (is_valid_coloring(graph, colors, k)) {
                /* Found a valid k-coloring! */
                result.found = true;
                result.chromatic_number = k;
                for (int i = 0; i < graph->num_vertices; i++) {
                    /* Store colors as 1-indexed for output */
                    result.coloring[i] = colors[i] + 1;
                }
                return result;
            }
            
            /* Generate next color assignment */
            more_assignments = next_color_assignment(colors, graph->num_vertices, k);
        }
        
        /* All k^V assignments tried, none valid - need more colors */
    }
    
    /* Exceeded max_k without finding valid coloring */
    result.aborted = true;
    snprintf(result.abort_reason, sizeof(result.abort_reason),
        "Exceeded maximum colors to try (%d)", max_k);
    return result;
}

/**
 * Print graph information summary.
 */
void print_graph_info(const Graph *graph) {
    printf("Graph Statistics:\n");
    printf("  Vertices: %d\n", graph->num_vertices);
    printf("  Edges: %d\n", graph->num_edges);
    
    /* Calculate and display maximum degree */
    int max_degree = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        int degree = 0;
        for (int j = 0; j < graph->num_vertices; j++) {
            degree += graph->adjacency[i][j];
        }
        if (degree > max_degree) {
            max_degree = degree;
        }
    }
    printf("  Maximum degree: %d\n", max_degree);
    printf("  Theoretical lower bound: %s\n", max_degree > 0 ? "at least 2" : "1 (empty graph)");
    printf("\n");
}

/**
 * Print the coloring results.
 */
void print_results(const ColoringResult *result, const Graph *graph) {
    printf("===================================\n");
    printf("Graph Coloring - Brute Force Search\n");
    printf("===================================\n\n");
    
    print_graph_info(graph);
    
    if (result->aborted) {
        printf("*** SEARCH ABORTED ***\n");
        printf("Reason: %s\n", result->abort_reason);
        printf("\nThe graph may be too large for brute force approach.\n");
        printf("Consider using the greedy algorithm for larger graphs.\n");
        return;
    }
    
    if (!result->found) {
        printf("*** NO SOLUTION FOUND ***\n");
        printf("Could not find a valid coloring within constraints.\n");
        return;
    }
    
    printf("*** RESULTS ***\n");
    printf("Chromatic number: %d\n", result->chromatic_number);
    printf("\nVertex coloring:\n");
    for (int i = 0; i < graph->num_vertices; i++) {
        printf("  Vertex %d: Color %d\n", i, result->coloring[i]);
    }
    
    printf("\nTime elapsed: %.6f seconds\n", result->time_elapsed);
    
    /* Provide context about the search */
    printf("\n--- Search Summary ---\n");
    printf("This solution is OPTIMAL (guaranteed minimum).\n");
    printf("All possible %d-colorings were examined.\n", result->chromatic_number);
}

/**
 * Print usage instructions.
 */
void print_usage(const char *program_name) {
    printf("Usage: %s <graph_file>\n\n", program_name);
    printf("Finds the chromatic number of a graph using brute force search.\n\n");
    printf("Arguments:\n");
    printf("  graph_file    Path to graph input file\n\n");
    printf("File format:\n");
    printf("  V E           (first line: vertices and edges)\n");
    printf("  u1 v1         (edge 1: vertex indices, 0-indexed)\n");
    printf("  u2 v2         (edge 2)\n");
    printf("  ...\n\n");
    printf("Example:\n");
    printf("  %s test-cases/small_graph.txt\n", program_name);
}

/**
 * Check if graph size is within practical limits.
 */
bool check_size_limits(int num_vertices, int max_k) {
    /* Warn if graph is likely too large for brute force */
    if (num_vertices > 12) {
        printf("WARNING: Graph has %d vertices.\n", num_vertices);
        printf("Brute force complexity is O(k^V × E).\n");
        printf("This may take a very long time or exceed memory limits.\n");
        printf("Consider using the greedy algorithm instead.\n\n");
        return false;
    }
    return true;
}

/**
 * Main entry point.
 */
int main(int argc, char *argv[]) {
    /* Check command line arguments */
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    /* Load graph from file */
    Graph graph;
    if (!load_graph_from_file(argv[1], &graph)) {
        return 1;
    }
    
    /* Warn about large graphs */
    check_size_limits(graph.num_vertices, MAX_COLORS_TO_TRY);
    
    /* Record start time for performance measurement */
    clock_t start_time = clock();
    
    /* Perform brute force search */
    ColoringResult result = find_chromatic_number(&graph, MAX_COLORS_TO_TRY, start_time);
    
    /* Record end time */
    clock_t end_time = clock();
    result.time_elapsed = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    /* Print results */
    print_results(&result, &graph);
    
    /* Return appropriate exit code */
    if (result.found) {
        return 0;
    } else {
        return 2;  /* No solution found */
    }
}
