# Author: Ishan Pranav
# Copyright (c) 2023 Ishan Pranav. All rights reserved.
# Licensed under the MIT License.

from itertools import combinations
from fileinput import input
from sympy import Line, Point
from time import time

def construct(p, dp):
    x, y, _ = p
    dx, dy, _ = dp
    
    return Line(Point(x, y), Point(x + dx, y + dy)), x, dx

def bounded(rx, x, dx):
    return (rx > x and dx > 0) or (rx < x and dx < 0)

items = []
min = 200000000000000
max = 400000000000001
start = time()

for line in input():
    p, dp = line.split(" @ ")
    x, y, z = [ int(pi) for pi in p.split(", ") ]
    dx, dy, dz = [ int(dpi) for dpi in dp.split(", ") ]
    items.append(((x, y, z), (dx, dy, dz)))

count = 0

for (p, dp), (q, dq) in combinations(items, 2):
    l, px, pdx = construct(p, dp)
    m, qx, qdx = construct(q, dq)
    intersection = l.intersection(m)
    
    if len(intersection) != 1:
        continue

    rx, ry = intersection[0]
    
    if min <= rx < max and min <= ry < max and \
        bounded(rx, px, pdx) and bounded(rx, qx, qdx):
        count += 1

print("24a", count, time() - start)
