# Ange's Greedy Graph Coloring Algorithm — Design Notes

**Status:** Algorithm designed, implementation pending  
**Algorithm Type:** Greedy Heuristic  
**Expected Complexity:** O(V² + E)

---

## Algorithm Overview

The greedy algorithm for graph coloring provides a **fast, polynomial-time** solution that approximates the minimum chromatic number. Unlike the brute force approach, it does **NOT guarantee optimality** — it may produce a valid coloring that uses more colors than the true chromatic number χ(G).

### Key Idea

Process vertices in a fixed order, assigning each vertex the **smallest available color** that doesn't conflict with already-colored neighbors.

### Trade-offs

| Pros | Cons |
|------|------|
| Fast execution — handles large graphs | May use more colors than necessary |
| Simple to implement and understand | Result depends on vertex ordering |
| Polynomial time complexity | No optimality guarantee |
| Often produces good results in practice | Worst case can use Δ+1 colors |

---

## Vertex Ordering Strategy

### Selected Approach: Sequential (0 to V-1)

**Decision:** Process vertices in natural order: 0, 1, 2, ..., V-1

**Rationale:**
- Simplicity — No preprocessing or sorting overhead
- Determinism — Same input always produces same output
- Easy to analyze and compare with other orderings

### Alternative Orderings Considered

| Ordering | Description | Pros | Cons |
|----------|-------------|------|------|
| **Degree-based (high to low)** | Process highest-degree vertices first | Often uses fewer colors | Requires sorting, O(V log V) |
| **Degree-based (low to high)** | Process lowest-degree vertices first | May help specific graph classes | Same sorting overhead |
| **Random** | Random permutation | Can escape bad local minima | Non-deterministic |
| **DSatur** | Dynamic saturation | Best known greedy heuristic | More complex, dynamic priority |

**References on Vertex Ordering:**
- Stack Overflow — Graph Vertex Ordering Degree Problem: https://stackoverflow.com/questions/64537001/graph-vertex-ordering-degree-problem-finding-a-k-ordering

---

## Greedy Color Selection Logic

### Core Procedure

For each vertex `v` in order (0 to V-1):

1. **Examine all neighbors** of `v` that have already been colored
2. **Collect the set of colors** used by those neighbors
3. **Find the smallest positive integer** (starting from 1) not in that set
4. **Assign that color** to vertex `v`

### Pseudocode

```
function greedyColoring(G = (V, E)):
    // Initialize: all vertices uncolored (color = 0)
    colors[0...V-1] = 0
    
    // Track which colors are used by neighbors
    used_colors = empty set
    
    for v = 0 to V-1:
        // Step 1: Check all neighbors of v
        used_colors.clear()
        
        for each neighbor u of v:
            if colors[u] != 0:           // u is already colored
                used_colors.add(colors[u])
        
        // Step 2: Find smallest available color
        c = 1
        while c is in used_colors:
            c = c + 1
        
        // Step 3: Assign color to v
        colors[v] = c
    
    // Return results
    max_color = maximum value in colors
    return (max_color, colors)
```

### Detailed Example

Consider a path graph: 0 — 1 — 2 — 3

| Step | Vertex | Colored Neighbors | Their Colors | Smallest Available | Assigned |
|------|--------|-------------------|--------------|-------------------|----------|
| 1 | 0 | None | ∅ | 1 | 1 |
| 2 | 1 | 0 | {1} | 2 | 2 |
| 3 | 2 | 1 | {2} | 1 | 1 |
| 4 | 3 | 2 | {1} | 2 | 2 |

**Result:** Uses 2 colors. Chromatic number = 2. ✓ Optimal!

---

## Time Complexity Analysis

### Deriving O(V² + E)

**Outer loop:** Iterate through all V vertices → **O(V)**

**Inner loop (checking neighbors):** For each vertex v, examine all its neighbors
- Total edge traversals across all vertices = 2E (each edge checked twice from both ends)
- This contributes **O(E)**

**Color selection:** Finding smallest available color
- In worst case, vertex has degree d, may need to check up to d+1 colors
- Using a boolean array of size Δ+1 (where Δ = max degree), this is O(d)
- Summed over all vertices: O(E)

**Adjacency check overhead:**
- For adjacency matrix: checking all neighbors is O(V) per vertex → O(V²) total
- For adjacency list: O(E) total

**Overall:** With adjacency matrix (as used in this project), complexity is **O(V² + E)**

Since E ≤ V² in simple graphs, this simplifies to **O(V²)** in dense graphs.

---

## Space Complexity

- **Color array:** O(V)
- **Used colors tracking:** O(Δ) where Δ is max degree, bounded by O(V)
- **Adjacency matrix:** O(V²)

**Total:** O(V²) for adjacency matrix representation

---

## Limitations and Non-Optimality

### Greedy Does NOT Guarantee χ(G)

**Counterexample:** Consider a graph where greedy ordering matters.

Graph: Two triangles sharing a vertex (bowtie shape)
- Vertices: 0, 1, 2, 3, 4
- Edges: (0,1), (1,2), (2,0), (0,3), (3,4), (4,0)

If processed in order 0, 1, 2, 3, 4:
- 0 gets color 1
- 1 gets color 2 (neighbor 0 has 1)
- 2 gets color 3 (neighbors 0,1 have 1,2)
- 3 gets color 2 (neighbor 0 has 1)
- 4 gets color 3 (neighbors 0,3 have 1,2)

**Greedy result:** 3 colors
**True χ(G):** 3 (still optimal here)

**Better counterexample:** A bipartite graph with "bad" ordering can force 3 colors even though χ(G) = 2.

### When Greedy Fails

- **Worst case:** Can use Δ + 1 colors where Δ is maximum degree
- **Example:** Some graphs exist where any ordering causes greedy to use > χ(G) colors
- **Brook's Theorem:** χ(G) ≤ Δ unless G is complete or an odd cycle

### Practical Note

For many real-world graphs (sparse, random, or with structure), greedy often produces near-optimal colorings. The gap between greedy result and χ(G) is usually small.

---

## References

### Primary Sources

1. **GeeksforGeeks — Greedy Algorithms:**
   https://www.geeksforgeeks.org/dsa/greedy-algorithms/
   
   Comprehensive tutorial on greedy approach fundamentals.

2. **Stack Overflow — Graph Vertex Ordering:**
   https://stackoverflow.com/questions/64537001/graph-vertex-ordering-degree-problem-finding-a-k-ordering
   
   Discussion of vertex ordering strategies for graph coloring.

### Academic References

3. **Welsh-Powell Algorithm:** Classical greedy with degree-based ordering
   - Sort vertices by degree (descending)
   - Apply greedy coloring
   - Often better than simple sequential ordering

4. **DSatur (Brelaz, 1979):** Dynamic ordering by saturation degree
   - Choose vertex with highest saturation (number of differently colored neighbors)
   - Break ties by degree
   - Generally best pure greedy heuristic

---

## Implementation Checklist (Future Work)

- [ ] Read graph from file (same format as brute force)
- [ ] Build adjacency matrix
- [ ] Implement sequential vertex ordering
- [ ] Implement color selection logic
- [ ] Track execution time
- [ ] Compare results with brute force on test cases
- [ ] Document any additional orderings tested

---

## Expected Output Format

```
===================================
Graph Coloring - Greedy Algorithm
===================================

Graph loaded: 8 vertices, 12 edges
Vertex ordering: Sequential (0 to V-1)

*** RESULTS ***
Colors used: 4
Note: This is a greedy approximation. True χ(G) may be lower.
Vertex coloring:
  Vertex 0: Color 1
  Vertex 1: Color 2
  Vertex 2: Color 1
  ...

Time elapsed: 0.000045 seconds
```
