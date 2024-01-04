# Author: Ishan Pranav
# Copyright (c) 2023 Ishan Pranav. All rights reserved.
# Licensed under the MIT License.

from fileinput import input
from sympy import Eq, solve, symbols
from time import time

start = time()
variables = symbols('rx ry rz rdx rdy rdz')
rx, ry, rz, rdx, rdy, rdz = variables
equations = []
    
for line in input():
    p, dp = line.split(" @ ")
    x, y, z = [ int(pi) for pi in p.split(", ") ]
    dx, dy, dz = [ int(dpi) for dpi in dp.split(", ") ]
    
    equations.append(Eq((rx - x) * (dy - rdy), (ry - y) * (dx - rdx)))
    equations.append(Eq((ry - y) * (dz - rdz), (rz - z) * (dy - rdy)))
    
solution = solve(equations, variables, dict = True)[0]
total = solution[rx] + solution[ry] + solution[rz]

print("24b", total, time() - start)
