// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Nondeterministic finite automata

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text;

namespace Day7;

internal static partial class Program
{
    private static int Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: Day12a <path>");

            return 1;
        }

        Run(args[0], out long min, out TimeSpan elapsed);
        Console.WriteLine("{0} : {1}", min, elapsed.TotalSeconds);

        return 0;
    }

    private static int ProcessLine(string line, StringBuilder builder, Dictionary<int, int> current, Dictionary<int, int> next)
    {
        int index = line.IndexOf(' ');

        if (index == -1)
        {
            throw new FormatException();
        }

        builder
            .Clear()
            .Append('.');
        current.Clear();
        next.Clear();

        current[0] = 1;

        foreach (string segment in line
            .Substring(index)
            .Split(','))
        {
            builder
                .Append('#', int.Parse(segment, CultureInfo.InvariantCulture))
                .Append('.');
        }

        foreach (char symbol in line.AsSpan(0, index))
        {
            foreach (int state in current.Keys)
            {
                switch (symbol)
                {
                    case '?':
                        if (state + 1 < builder.Length)
                        {
                            next[state + 1] = next.GetValueOrDefault(state + 1) + current[state];
                        }

                        if (builder[state] == '.')
                        {
                            next[state] = next.GetValueOrDefault(state) + current[state];
                        }
                        break;

                    case '.':
                        if (state + 1 < builder.Length && builder[state + 1] == '.')
                        {
                            next[state + 1] = next.GetValueOrDefault(state + 1) + current[state];
                        }

                        if (builder[state] == '.')
                        {
                            next[state] = next.GetValueOrDefault(state) + current[state];
                        }
                        break;

                    case '#':
                        if (state + 1 < builder.Length && builder[state + 1] == '#')
                        {
                            next[state + 1] = next.GetValueOrDefault(state + 1) + current[state];
                        }
                        break;
                }
            }

            current = next;
            next = new Dictionary<int, int>();
        }

        return current.GetValueOrDefault(builder.Length - 1) + current.GetValueOrDefault(builder.Length - 2);
    }

    private static void Run(string path, out long total, out TimeSpan elapsed)
    {
        using StreamReader reader = File.OpenText(path);

        string? line;
        Stopwatch stopwatch = Stopwatch.StartNew();
        StringBuilder builder = new StringBuilder();
        Dictionary<int, int> current = new Dictionary<int, int>();
        Dictionary<int, int> next = new Dictionary<int, int>();

        total = 0;

        while ((line = reader.ReadLine()) != null)
        {
            total += ProcessLine(line, builder, current, next);
        }

        elapsed = stopwatch.Elapsed;
    }
}
