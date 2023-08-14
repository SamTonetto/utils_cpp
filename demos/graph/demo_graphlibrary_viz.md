# Demo - Graphlibrary visualization

This example shows how to use the graph library to generate graphs and write
them to a file. The graphs are written to a "show_graph_outputs" folder in the DOT format, and they can can be
visualized using Graphviz by running

```
neato -Tpng <graph>.dot -o <graph>.png
```

Source code:

\include demo_graphlibrary_viz.cpp
