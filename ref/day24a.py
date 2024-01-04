from itertools import combinations
from fileinput import input
from sympy import Line, Point
from time import time

def construct(p, dp):
    x, y, _ = p
    dx, dy, _ = dp
    
    return Line(Point(x, y), Point(x + dx, y + dy)), x, dx

def bounded(rx, x, dx) -> bool:
    return (rx > x and dx > 0) or (rx < x and dx < 0)

items = []
lo = 200000000000000
hi = 400000000000000
start = time()

for line in input():
    p, dp = line.split(" @ ")
    x, y, z = [ int(pi) for pi in p.split(", ") ]
    dx, dy, dz = [ int(dpi) for dpi in dp.split(", ") ]
    items.append(((x, y, z), (dx, dy, dz)))

count = 0

for (p, dp), (q, dq) in combinations(items, 2):
    l, x1, dx1 = construct(p, dp)
    m, x2, dx2 = construct(q, dq)
    intersection = l.intersection(m)
    
    if len(intersection) != 1:
        continue

    rx, ry = intersection[0]
    
    if lo <= rx <= hi and lo <= ry <= hi and \
        bounded(rx, x1, dx1) and bounded(rx, x2, dx2):
        count += 1

print("24a", count, time() - start)
