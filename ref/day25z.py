# Author: Ishan Pranav
# Copyright (c) 2023 Ishan Pranav. All rights reserved.
# Licensed under the MIT License.

from fileinput import input
from itertools import combinations
from networkx import Graph, minimum_cut
from time import time

start = time()
graph = Graph()

for line in input():
    source, targets = line.split(": ")
    
    for target in targets.split():
        graph.add_edge(source, target, capacity = 1)

for u, v in combinations(graph.nodes, 2):
    cuts, partition = minimum_cut(graph, u, v)
    
    if cuts == 3:
        break

total = len(partition[0]) * len(partition[1])

print("25z", total, time() - start)
