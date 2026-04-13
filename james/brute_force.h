/**
 * brute_force.h - Header file for brute force graph coloring
 * 
 * CMSC 142 - Minimum Graph Coloring Project
 * Author: James
 * 
 * This module implements a brute force search for the chromatic number
 * of a graph using exhaustive enumeration of color assignments.
 * 
 * Reference: https://www.geeksforgeeks.org/graph-coloring-applications/
 */

#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * CONSTANTS AND LIMITS
 * ============================================================================
 * These defines control the behavior and safety limits of the algorithm.
 * Modify these to adjust performance characteristics.
 */

/** Maximum number of vertices the program can handle */
#define MAX_VERTICES 20

/** Maximum number of edges the program can handle */
#define MAX_EDGES 200

/** 
 * Maximum chromatic number to try before giving up.
 * Safety cap to prevent infinite loops on large graphs.
 */
#define MAX_COLORS_TO_TRY 15

/** 
 * Maximum execution time in seconds.
 * If exceeded, the program will abort with a warning.
 */
#define MAX_EXECUTION_TIME_SECONDS 60

/** 
 * Warning threshold in seconds.
 * If exceeded, the program will display a warning but continue.
 */
#define WARNING_TIME_THRESHOLD_SECONDS 5

/* ============================================================================
 * DATA STRUCTURES
 * ============================================================================
 */

/**
 * Structure to represent a graph using adjacency matrix representation.
 * 
 * Using adjacency matrix allows O(1) edge existence checks,
 * which is efficient for the validation step in brute force search.
 */
typedef struct {
    int num_vertices;              /**< Number of vertices in the graph (V) */
    int num_edges;                 /**< Number of edges in the graph (E) */
    int adjacency[MAX_VERTICES][MAX_VERTICES];  /**< Adjacency matrix: 1 if edge exists, 0 otherwise */
} Graph;

/**
 * Structure to hold the results of the graph coloring search.
 */
typedef struct {
    int chromatic_number;          /**< The minimum number of colors found */
    int coloring[MAX_VERTICES];    /**< Color assignment for each vertex (1-indexed) */
    double time_elapsed;           /**< Time taken to find the solution (seconds) */
    bool found;                    /**< Whether a valid coloring was found */
    bool aborted;                  /**< Whether search was aborted due to limits */
    char abort_reason[256];        /**< Reason for abortion if applicable */
} ColoringResult;

/* ============================================================================
 * FUNCTION DECLARATIONS
 * ============================================================================
 */

/**
 * Load a graph from a text file.
 * 
 * File format:
 *   V E              (first line: number of vertices and edges)
 *   u1 v1            (subsequent lines: edge endpoints, 0-indexed)
 *   u2 v2
 *   ...
 * 
 * @param filename Path to the input file
 * @param graph Pointer to Graph structure to populate
 * @return true if successful, false if error occurred
 */
bool load_graph_from_file(const char *filename, Graph *graph);

/**
 * Validate if a color assignment is proper (no adjacent vertices share color).
 * 
 * @param graph Pointer to the graph structure
 * @param colors Array of color assignments (0 to k-1 or 1 to k)
 * @param k Number of colors used in this assignment
 * @return true if valid proper coloring, false otherwise
 */
bool is_valid_coloring(const Graph *graph, const int colors[], int k);

/**
 * Check if two adjacent vertices have the same color.
 * 
 * @param graph Pointer to the graph structure
 * @param colors Array of color assignments
 * @return true if conflict exists, false otherwise
 */
bool has_adjacent_conflict(const Graph *graph, const int colors[]);

/**
 * Generate the next color assignment (base-k increment).
 * 
 * Treats the color assignment as a base-k number and increments it.
 * Returns false when all combinations have been tried (overflow).
 * 
 * Example for V=3, k=2:
 *   [0,0,0] -> [0,0,1] -> [0,1,0] -> [0,1,1] -> [1,0,0] -> ... -> [1,1,1] -> false
 * 
 * @param colors Array of current color assignments (modified in place)
 * @param num_vertices Number of vertices
 * @param k Number of colors (base)
 * @return true if next assignment generated, false if all tried
 */
bool next_color_assignment(int colors[], int num_vertices, int k);

/**
 * Find the chromatic number using brute force search.
 * 
 * Algorithm:
 *   For k = 1 to max_k:
 *     Generate all k^V color assignments
 *     Check each for validity
 *     Return first valid assignment found
 * 
 * @param graph Pointer to the graph structure
 * @param max_k Maximum number of colors to try (safety limit)
 * @param start_time Clock value when search began (for timeout checking)
 * @return ColoringResult structure with results and metadata
 */
ColoringResult find_chromatic_number(const Graph *graph, int max_k, clock_t start_time);

/**
 * Print the coloring results to stdout.
 * 
 * @param result Pointer to the ColoringResult structure
 * @param graph Pointer to the graph (for vertex count)
 */
void print_results(const ColoringResult *result, const Graph *graph);

/**
 * Print graph statistics and summary.
 * 
 * @param graph Pointer to the graph structure
 */
void print_graph_info(const Graph *graph);

/**
 * Display usage information and help message.
 * 
 * @param program_name Name of the executable (argv[0])
 */
void print_usage(const char *program_name);

/**
 * Check if the graph size is within practical limits for brute force.
 * 
 * Provides a warning if the graph is likely too large.
 * 
 * @param num_vertices Number of vertices
 * @param max_k Maximum colors to try
 * @return true if within limits, false if excessive
 */
bool check_size_limits(int num_vertices, int max_k);

#endif /* BRUTE_FORCE_H */
