# Demo - Writing to graphviz

Creates a graph with Vertex and Edge properties, and writes it to a DOT file.

In particular, it demonstrates how to pick internal Vertex or Edge properties
to visualize, as well as dynamically create properties to visualize.

The DOT file can be visualized with Graphviz, e.g.:

```
neato -Tpng <graph>.dot -o <graph>.png
```

Source code:

   \include demo_write_graphviz.cpp
