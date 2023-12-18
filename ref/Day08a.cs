// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom implementation
// Pack the three-character node identifiers into one value

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Day7;

internal static partial class Program
{
    private static int Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: Day08a <path>");

            return 1;
        }

        Run(args[0], out long min, out TimeSpan elapsed);
        Console.WriteLine("{0} : {1}", min, elapsed.TotalSeconds);

        return 0;
    }

    private static void Run(string path, out long total, out TimeSpan elapsed)
    {
        using StreamReader reader = File.OpenText(path);

        Stopwatch stopwatch = Stopwatch.StartNew();

        string? directions = reader.ReadLine();

        if (directions == null || reader.ReadLine() == null)
        {
            throw new FormatException();
        }

        string? line;
        Dictionary<Vertex, (Vertex left, Vertex right)> graph = new Dictionary<Vertex, (Vertex, Vertex)>();

        while ((line = reader.ReadLine()) != null)
        {
            if (line.Length != 16)
            {
                throw new FormatException();
            }

            Vertex node = new Vertex(line[0], line[1], line[2]);
            Vertex left = new Vertex(line[7], line[8], line[9]);
            Vertex right = new Vertex(line[12], line[13], line[14]);

            graph.Add(node, (left, right));
        }

        total = 0;

        Vertex current = new Vertex('A', 'A', 'A');
        Vertex end = new Vertex('Z', 'Z', 'Z');
        int direction = 0;

        while (current != end)
        {
            if (directions[direction] == 'L')
            {
                current = graph[current].left;
            }
            else
            {
                current = graph[current].right;
            }

            total++;
            direction++;

            if (direction == directions.Length)
            {
                direction = 0;
            }
        }

        stopwatch.Stop();

        elapsed = stopwatch.Elapsed;
    }
}

internal readonly struct Vertex : IEquatable<Vertex>
{
    private readonly char _a;
    private readonly char _b;
    private readonly char _c;

    public Vertex(char a, char b, char c)
    {
        _a = a;
        _b = b;
        _c = c;
    }

    public override string ToString()
    {
        return $"{_a}{_b}{_c}";
    }

    public bool Equals(Vertex other)
    {
        return _a == other._a &&
            _b == other._b &&
            _c == other._c;
    }

    public override bool Equals(object? obj)
    {
        return obj is Vertex other && Equals(other);
    }

    public override int GetHashCode()
    {
        return ToString().GetHashCode();
    }

    public static bool operator ==(Vertex left, Vertex right)
    {
        return left.Equals(right);
    }

    public static bool operator !=(Vertex left, Vertex right)
    {
        return !(left == right);
    }
}
