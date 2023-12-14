// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom implementation
// Pack the three-character node identifiers into one value

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace Day7;

internal static partial class Program
{
    private static int Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: Day8a <path>");

            return 1;
        }

        Console.OutputEncoding = Encoding.Unicode;

        Run(args[0], out long min, out TimeSpan elapsed);
        Console.WriteLine("{0} : {1}", min, elapsed.TotalSeconds);

        return 0;
    }

    private static void Run(string path, out long total, out TimeSpan elapsed)
    {
        using StreamReader reader = File.OpenText(path);

        Stopwatch stopwatch = Stopwatch.StartNew();

        string? directions = reader.ReadLine();

        if (directions == null || reader.ReadLine() ==  null)
        {
            throw new FormatException();
        }

        string? line;
        Dictionary<Node, (Node left, Node right)> graph = new Dictionary<Node, (Node, Node)>();

        while ((line = reader.ReadLine()) != null)
        {
            if (line.Length != 16)
            {
                throw new FormatException();
            }

            Node node = new Node(line.AsSpan(start: 0, length: 3));
            Node left = new Node(line.AsSpan(start: 7, length: 3));
            Node right = new Node(line.AsSpan(start: 12, length: 3));

            graph.Add(node, (left, right));
        }

        total = 0;

        Node current = new Node("AAA");
        Node end = new Node("ZZZ");
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

internal readonly struct Node : IEquatable<Node>
{
    private const int CharBits = 16;

    private readonly ulong _id;

    public Node(ReadOnlySpan<char> value)
    {
        _id = ((ulong)value[0] << (CharBits * 2)) 
            | ((ulong)value[1] << CharBits) 
            | value[2];
    }

    public override string ToString()
    {
        const ulong ByteMask = (1 << CharBits) - 1;

        return new string(new char[]
        {
            (char)((_id >> (CharBits * 2)) & ByteMask),
            (char)((_id >> CharBits) & ByteMask),
            (char)(_id & ByteMask)
        });
    }

    public bool Equals(Node other)
    {
        return _id == other._id;
    }

    public override bool Equals(object? obj)
    { 
        return obj is Node other && Equals(other);
    }

    public override int GetHashCode()
    {
        return _id.GetHashCode();
    }

    public static bool operator ==(Node left, Node right)
    {
        return left.Equals(right);
    }

    public static bool operator !=(Node left, Node right)
    {
        return !(left == right);
    }
}
