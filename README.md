# minimum graph coloring - cmsc 142

project implementing brute force and greedy approaches for the minimum graph coloring problem (np-complete).

## what is graph coloring?

assign colors to vertices such that no two adjacent vertices share the same color. the **chromatic number** χ(g) is the minimum number of colors needed.

## project structure

```
minimum-graph-coloring/
├── README.md              # this file
├── james/                 # brute force implementation
│   ├── brute_force.c      # complete solution
│   └── NOTES.md           # implementation notes
├── ange/                  # greedy algorithm (planned)
│   └── NOTES.md           # algorithm documentation
├── test-cases/            # sample graphs
│   ├── small_graph.txt    # 4 vertices, χ = 3
│   ├── medium_graph.txt   # 8 vertices
│   ├── complete_k4.txt    # 4 vertices, χ = 4
│   └── bipartite.txt      # 6 vertices, χ = 2
└── results/               # output directory
```

## algorithms

### brute force (james)
exhaustive search over all k^v color assignments.
- **guarantees optimal** χ(g)
- **time:** o(k^v × e)
- **limit:** viable for v ≤ 12

### greedy (ange - planned)
fast heuristic assigning smallest available color to each vertex.
- **fast:** o(v² + e)
- **no optimality guarantee**
- **good for:** large graphs, approximation

see `james/NOTES.md` and `ange/NOTES.md` for details.

## how to compile and run

```bash
cd james
gcc -o brute_force brute_force.c -lm
cd ..
./james/brute_force test-cases/small_graph.txt
```

### input file format
```
v e              # vertices and edges
u1 v1            # edge 1 (0-indexed)
u2 v2            # edge 2
...
```

## test case results

| test case | vertices | edges | expected χ(g) |
|-----------|----------|-------|---------------|
| `small_graph.txt` | 4 | 4 | 3 |
| `medium_graph.txt` | 8 | 12 | 3 |
| `complete_k4.txt` | 4 | 6 | 4 |
| `bipartite.txt` | 6 | 6 | 2 |

## real-world applications

- **register allocation** (compilers): map variables to cpu registers
- **map coloring**: adjacent regions need different colors
- **scheduling**: exams/classes with no student conflicts
- **frequency assignment**: wireless transmitters without interference

## authors

- **james** — brute force implementation
- **ange** — greedy algorithm design

## references

- https://www.geeksforgeeks.org/graph-coloring-applications/
- https://www.geeksforgeeks.org/dsa/greedy-algorithms/
