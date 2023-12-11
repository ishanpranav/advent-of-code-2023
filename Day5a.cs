// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Brute-force with binary search
// Time complexity: O(n * m * log(p)) where
//      n is the number of seeds,
//      m is the number of functions, and
//      p is the number of ranges in the largest map.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;

namespace Day5;

internal static partial class Program
{
    private static int Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: Day5a <path>");

            return 1;
        }

        Run(args[0], out long min, out TimeSpan elapsed);
        Console.WriteLine("{0} : {1}", min, elapsed.TotalSeconds);

        return 0;
    }

    private static void TransformSeeds(Function function, List<long> seeds)
    {
        function.SortRanges();

        for (int i = 0; i < seeds.Count; i++)
        {
            seeds[i] = function.Transform(seeds[i]);
        }

        function.ClearRanges();
    }

    private static void Run(string path, out long min, out TimeSpan elapsed)
    {
        using StreamReader reader = File.OpenText(path);

        Stopwatch stopwatch = Stopwatch.StartNew();

        string? line = reader.ReadLine();

        if (line == null)
        {
            throw new FormatException();
        }

        string[] tokens = line.Split(' ');
        Function current = new Function();
        List<long> seeds = new List<long>();

        for (int i = 1; i < tokens.Length; i++)
        {
            seeds.Add(long.Parse(tokens[i], CultureInfo.InvariantCulture));
        }

        while ((line = reader.ReadLine()) != null)
        {
            tokens = line.Split(' ');

            switch (tokens.Length)
            {
                case 2:
                    TransformSeeds(current, seeds);
                    break;

                case 3:
                    current.AddRange(new Range(
                        long.Parse(tokens[0], CultureInfo.InvariantCulture),
                        long.Parse(tokens[1], CultureInfo.InvariantCulture),
                        long.Parse(tokens[2], CultureInfo.InvariantCulture)));
                    break;
            }
        }

        TransformSeeds(current, seeds);

        min = seeds.Min();

        stopwatch.Stop();

        elapsed = stopwatch.Elapsed;
    }
}

internal sealed class Function
{
    private readonly List<Range> _ranges = new List<Range>();

    public void AddRange(Range item)
    {
        _ranges.Add(item);
    }

    public void SortRanges()
    {
        _ranges.Sort();
    }

    public void ClearRanges()
    {
        _ranges.Clear();
    }

    private int BinarySearch(long value)
    {
        int lo = 0;
        int hi = _ranges.Count - 1;

        while (lo <= hi)
        {
            int i = lo + ((hi - lo) / 2);
            long other = _ranges[i].SourceOffset;

            if (other == value)
            {
                return i;
            }

            if (other > value)
            {
                hi = i - 1;
            }
            else
            {
                lo = i + 1;
            }
        }

        if (hi > 0)
        {
            return hi;
        }

        return 0;
    }

    public long Transform(long input)
    {
        if (_ranges.Count == 0)
        {
            return input;
        }

        Range range = _ranges[BinarySearch(input)];
        long offset = range.SourceOffset;

        if (input >= offset && input < offset + range.Length)
        {
            return input - offset + range.DestinationOffset;
        }

        return input;
    }
}

internal sealed class Range : IComparable<Range>
{
    public Range(long destinationOffset, long sourceOffset, long length)
    {
        DestinationOffset = destinationOffset;
        SourceOffset = sourceOffset;
        Length = length;
    }

    public long DestinationOffset { get; }
    public long SourceOffset { get; }
    public long Length { get; }

    public int CompareTo(Range? other)
    {
        if (other == null)
        {
            return 1;
        }

        return SourceOffset.CompareTo(other.SourceOffset);
    }

    public override string ToString()
    {
        return $"({DestinationOffset}, {SourceOffset}, {Length})";
    }
}
