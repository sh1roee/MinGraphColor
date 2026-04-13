# Minimum Graph Coloring - CMSC 142 Project

This project implements and compares two approaches to solving the **Minimum Graph Coloring** problem, a classic NP-complete problem in graph theory and computer science.

---

## Problem Statement

### What is Graph Coloring?

A **proper vertex coloring** of a graph assigns colors to vertices such that **no two adjacent vertices share the same color**. The **chromatic number** χ(G) is the **minimum number of colors** needed to properly color the graph G.

### Formal Definition

Given:
- A graph G = (V, E) with vertices V and edges E
- A set of colors {1, 2, ..., k}

Find:
- The smallest k such that every vertex is assigned a color and no edge connects vertices of the same color.

### Why It's Important

The Minimum Graph Coloring problem is:
- **NP-complete** — No known polynomial-time algorithm exists for general graphs
- **Practically significant** — Has direct applications in resource allocation and scheduling
- **Theoretically interesting** — Bridges discrete mathematics, algorithms, and complexity theory

---

## Project Structure

```
minimum-graph-coloring/
├── README.md              # This file — full project documentation
├── james/                 # James's Brute Force implementation
│   ├── brute_force.c      # Complete brute force solution
│   ├── brute_force.h      # Header file with declarations
│   └── NOTES.md           # Implementation notes and analysis
├── ange/                  # Ange's Greedy implementation (planned)
│   └── NOTES.md           # Algorithm documentation (no code yet)
├── test-cases/            # Sample graph inputs
│   ├── small_graph.txt    # 4 vertices, triangle + edge
│   ├── medium_graph.txt   # 8 vertices, moderately connected
│   ├── complete_k4.txt    # Complete graph K4 (χ = 4)
│   └── bipartite.txt      # Bipartite graph (χ = 2)
└── results/               # Output directory (empty, tracked via .gitkeep)
```

---

## Algorithms Implemented

### 1. Brute Force (James)

**Approach:** Exhaustive search over all possible color assignments.

**Algorithm Overview:**
1. Start with k = 1 color
2. Generate ALL possible k^V color assignments
3. For each assignment, validate by checking all edges
4. If a valid assignment is found, k is the chromatic number
5. If no valid assignment exists, increment k and repeat

**Characteristics:**
- **Guarantees optimal solution** — Will always find χ(G)
- **Exponential time** — O(k^V × E) per k value
- **Practical limit** — Only viable for small graphs (V ≤ 12)

See `james/NOTES.md` for detailed complexity analysis.

### 2. Greedy Algorithm (Ange)

**Approach:** Fast heuristic that may not find optimal solution.

**Algorithm Overview:**
1. Process vertices in order (0 to V-1)
2. For each vertex, assign the smallest available color
3. Ensure no neighbor shares the assigned color
4. Continue until all vertices are colored

**Characteristics:**
- **Fast** — O(V² + E) time complexity
- **No optimality guarantee** — May use more colors than χ(G)
- **Good approximation** — Works well for many practical cases

See `ange/NOTES.md` for pseudocode and detailed documentation.

---

## How to Compile and Run

### Prerequisites

- C compiler (gcc or clang)
- Standard C library (libc)

### Compiling the Brute Force Solution

```bash
# Navigate to the james directory
cd james

# Compile with GCC
gcc -o brute_force brute_force.c -Wall -O2

# Or with Clang
clang -o brute_force brute_force.c -Wall -O2

# Return to project root
cd ..
```

### Running the Brute Force Program

```bash
# Run with a test case
./james/brute_force test-cases/small_graph.txt

# Run with another test case
./james/brute_force test-cases/complete_k4.txt
```

### Input File Format

Test files follow this format:
```
V E              # First line: number of vertices and edges
u1 v1            # Edge 1: vertices u and v (0-indexed)
u2 v2            # Edge 2
...
uE vE            # Edge E
```

**Example:**
```
4 5
0 1
1 2
2 0
0 3
3 1
```

This creates a graph with 4 vertices and 5 edges.

---

## Interpreting Results

### Expected Output Format

```
===================================
Graph Coloring - Brute Force Search
===================================

Graph loaded: 4 vertices, 5 edges

Searching for chromatic number...

*** RESULTS ***
Chromatic number: 3
Vertex coloring:
  Vertex 0: Color 1
  Vertex 1: Color 2
  Vertex 2: Color 3
  Vertex 3: Color 1

Time elapsed: 0.000123 seconds
```

### Understanding the Output

- **Chromatic number**: The minimum k where a valid coloring exists
- **Vertex coloring**: The specific color assigned to each vertex
- **Time elapsed**: CPU time spent searching (may be near zero for tiny graphs)

### Expected Results for Test Cases

| Test Case | Vertices | Edges | Expected χ(G) | Description |
|-----------|----------|-------|---------------|-------------|
| `small_graph.txt` | 4 | 4 | 3 | Triangle (needs 3) + isolated edge |
| `medium_graph.txt` | 8 | 12 | 3-4 | Moderately connected graph |
| `complete_k4.txt` | 4 | 6 | 4 | Every vertex connected to every other |
| `bipartite.txt` | 6 | 7 | 2 | No odd-length cycles |

---

## Real-World Applications

Graph coloring has direct applications across computer science and engineering:

### 1. Register Allocation (Compilers)

**Problem:** Map program variables to CPU registers efficiently.

**Modeling:**
- Vertices = Program variables
- Edges = Variables that are "live" simultaneously
- Colors = CPU registers

**Goal:** Use fewest registers (colors) while ensuring no conflicts occur at runtime.

**Significance:** Modern compilers (GCC, LLVM) use graph coloring to optimize code execution. Fewer register spills = faster programs.

### 2. Map Coloring

**Problem:** Color countries/regions on a map so adjacent regions have different colors.

**Historical Note:** This problem inspired the Four Color Theorem, which states that any planar map can be colored with at most 4 colors.

**Modern Use:** Cartography, geographic information systems (GIS), frequency planning.

### 3. Scheduling (Exam/Class Timetables)

**Problem:** Schedule exams or classes so no student has conflicts.

**Modeling:**
- Vertices = Exams/classes
- Edges = Pairs of exams with shared students
- Colors = Time slots

**Goal:** Minimize time slots while respecting all student enrollments.

**Institution Use:** Universities worldwide use graph coloring to optimize exam schedules, reducing student stress and room allocation costs.

### 4. Frequency Assignment (Wireless Networks)

**Problem:** Assign radio frequencies to transmitters without interference.

**Modeling:**
- Vertices = Transmitters/antenna towers
- Edges = Transmitters within interference range
- Colors = Frequency bands

**Goal:** Use minimum frequencies while maintaining signal quality.

**Industry Impact:** Cellular networks (4G, 5G), WiFi channel allocation, satellite communication.

### 5. Other Applications

- **Sudoku solving** — Constraint satisfaction modeled as graph coloring
- **Pattern matching** — Textile manufacturing and circuit board design
- **Data mining** — Clustering and classification problems
- **Parallel computing** — Task scheduling on multi-core processors

---

## Comparison: Brute Force vs Greedy

| Aspect | Brute Force | Greedy |
|--------|-------------|--------|
| **Optimality** | ✓ Always finds χ(G) | ✗ May use extra colors |
| **Speed** | Slow (exponential) | Fast (polynomial) |
| **Complexity** | O(k^V × E) | O(V² + E) |
| **Use Case** | Small graphs, verification | Large graphs, approximation |
| **Max Viable Size** | ~12 vertices | Thousands of vertices |

**Practical Strategy:** Use greedy for large instances, verify critical cases with brute force.

---

## References and Further Reading

### Algorithm Sources

- GeeksforGeeks — Graph Coloring Applications: https://www.geeksforgeeks.org/graph-coloring-applications/
- GeeksforGeeks — Greedy Algorithms: https://www.geeksforgeeks.org/dsa/greedy-algorithms/

### Complexity Theory

- Karp's 21 NP-complete Problems — Proves graph coloring is NP-complete
- Approximation Algorithms — Greedy provides 2-approximation for some graph classes

### Textbooks

- Cormen, Leiserson, Rivest, Stein — *Introduction to Algorithms* (CLRS), Chapter on Graph Algorithms
- Diestel — *Graph Theory* — Formal treatment of chromatic number

---

## License

Academic project for CMSC 142 (Algorithms and Complexity).

## Authors

- **James** — Brute Force implementation
- **Ange** — Greedy algorithm design

---

## Build Status

| Component | Status |
|-----------|--------|
| Project Structure | ✓ Complete |
| README.md | ✓ Complete |
| James (Brute Force) | ✓ Implemented |
| Ange (Greedy) | Planned |
| Test Cases | ✓ Complete |
