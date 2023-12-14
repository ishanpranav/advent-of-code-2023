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
        Dictionary<string, (string left, string right)> graph = new Dictionary<string, (string, string)>();

        while ((line = reader.ReadLine()) != null)
        {
            if (line.Length != 16)
            {
                throw new FormatException();
            }

            string node = line.Substring( 0, length: 3);
            string left = line.Substring( 7, length: 3);
            string right = line.Substring( 12, length: 3);

            graph.Add(node, (left, right));
        }

        total = 0;

        string current = "AAA";
        string end = "ZZZ";
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
