# Demo - Bit-matrix adjacency matrix

The Bit-matrix adjacency matrix stores the adjacency matrix elements in a contiguous array of 64-bit unsigned integers.

So, for example, the adjacency matrix

  0 0 1
  0 0 0
  1 0 0

would be stored using 3 uint64_ts, one for each row:

- Row 0: r0 = 0000000000000000000000000000000000000000000000000000000000000100
- Row 1: r1 = 0000000000000000000000000000000000000000000000000000000000000000
- Row 2: r2 = 0000000000000000000000000000000000000000000000000000000000000001

So that accessing element (0,2) of the adjacency matrix is equivalent to returning the result of (r0 >> 2) & 1ULL, which is 1.

If the number of vertices in the graph is greater than 64, then each row is broken up into multiple uint64s.

So for example, in a 100-vertex graph, the adjacency matrix could comprise of 200 64-bit unsigned integers:

  adjmat = [row0_0, row0_1, row1_0, row1_1, row2_0, row2_1, ..., row_99_0, row_99_1]

where for each 'x', row_x_0 stores the adjacency information for the first 64 vertices, and row_x_1 store the remainder. This allows the adjacency matrix to be extremely memory efficient, while also performing well on dense graphs.

Note that everything is still stored contiguously, since this is built upon the contiguous matrix class.

This example show how to use the BitAdjmat class.

Source code:

  \include demo_bitadjmat.cpp
