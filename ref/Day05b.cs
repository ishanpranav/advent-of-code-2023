// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;

namespace Day5;

internal static class Program
{
    private static int Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: Day05b <path>");

            return 1;
        }

        Run(args[0], out long min, out TimeSpan elapsed);
        Console.WriteLine("{0} : {1}", min, elapsed.TotalSeconds);

        return 0;
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
        List<Interval> seeds = new List<Interval>();

        for (int i = 1; i < tokens.Length - 1; i += 2)
        {
            long offset = long.Parse(tokens[i], CultureInfo.InvariantCulture);

            seeds.Add(new Interval(
                offset,
                offset + long.Parse(
                    tokens[i + 1],
                    CultureInfo.InvariantCulture)));
        }

        Function? current = null;
        Function? composite = null;

        while ((line = reader.ReadLine()) != null)
        {
            tokens = line.Split(' ');

            switch (tokens.Length)
            {
                case 2:
                    if (current == null)
                    {
                        current = new Function();

                        break;
                    }

                    if (current.Count == 0)
                    {
                        break;
                    }

                    current.FillRanges();

                    if (composite == null)
                    {
                        composite = current;
                        current = new Function();

                        break;
                    }

                    composite = Function.Compose(composite, current);

                    current.ClearRanges();

                    break;

                case 3:
                    if (current == null)
                    {
                        throw new FormatException();
                    }

                    current.AddRange(new Range(
                        long.Parse(tokens[0], CultureInfo.InvariantCulture),
                        long.Parse(tokens[1], CultureInfo.InvariantCulture),
                        long.Parse(tokens[2], CultureInfo.InvariantCulture)));
                    break;
            }
        }

        if (composite == null)
        {
            throw new FormatException();
        }

        if (current != null && current.Count > 0)
        {
            current.FillRanges();

            composite = Function.Compose(composite, current);

            current.ClearRanges();
        }

        min = long.MaxValue;

        if (composite.Ranges.Count > 0)
        {
            foreach (Interval seed in seeds)
            {
                long seedMin = seed.Min;
                long seedMax = seed.Max;

                foreach (Range range in composite.Ranges)
                {
                    long rangeMin = range.SourceOffset;

                    if (rangeMin < seedMin || seedMax < rangeMin + range.Length)
                    {
                        continue;
                    }

                    long result = Math.Max(rangeMin, seedMin) - range.SourceOffset + range.DestinationOffset;

                    if (result < min)
                    {
                        min = result;
                    }
                }
            }
        }

        stopwatch.Stop();

        elapsed = stopwatch.Elapsed;
    }
}

internal sealed class Interval
{
    public Interval(long min, long max)
    {
        Min = min;
        Max = max;
    }

    public long Min { get; }
    public long Max { get; }
}

internal sealed class Function
{
    private readonly List<Range> _ranges = new List<Range>();

    public int Count
    {
        get
        {
            return _ranges.Count;
        }
    }

    public IReadOnlyCollection<Range> Ranges
    {
        get
        {
            return _ranges;
        }
    }

    public void AddRange(Range item)
    {
        _ranges.Add(item);
    }

    public void SortRanges()
    {
        _ranges.Sort();
    }

    public void FillRanges()
    {
        if (_ranges.Count == 0)
        {
            _ranges.Add(new Range(0, 0, long.MaxValue));

            return;
        }

        SortRanges();

        Range[] view = new Range[_ranges.Count];

        _ranges.CopyTo(view);
        _ranges.Clear();

        Range first = view[0];
        long min = first.SourceOffset;

        if (min != 0)
        {
            _ranges.Add(Range.Identity(min: 0, min));
        }

        _ranges.Add(first);

        for (int i = 1; i < view.Length; i++)
        {
            Range current = view[i];
            Range previous = view[i - 1];
            long previousMax = previous.SourceOffset + previous.Length;
            long currentMin = current.SourceOffset;
            long difference = currentMin - previousMax;

            if (difference > 1)
            {
                _ranges.Add(Range.Identity(previousMax + 1, currentMin + current.Length));
            }

            _ranges.Add(current);
        }

        Range last = _ranges[_ranges.Count - 1];
        long lastMax = last.SourceOffset + last.Length;

        if (lastMax < long.MaxValue)
        {
            _ranges.Add(Range.Identity(lastMax + 1, long.MaxValue));
        }
    }

    public void ClearRanges()
    {
        _ranges.Clear();
    }

    public static Function Compose(Function f, Function g)
    {
        Function result = new Function();

        foreach (Range a in f._ranges)
        {
            foreach (Range b in g._ranges)
            {
                long aMin = a.SourceOffset;
                long aMax = aMin + a.Length;
                long bMin = b.SourceOffset + a.SourceOffset - a.DestinationOffset;
                long bMax = bMin + b.Length;

                if (aMax <= bMin || bMax <= aMin)
                {
                    continue;
                }

                result.AddRange(Range.FromInterval(
                    Math.Max(aMin, bMin),
                    Math.Min(aMax, bMax),
                    a.DestinationOffset + b.DestinationOffset
                    - a.SourceOffset - b.SourceOffset));
            }
        }

        return result;
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

    public static Range FromInterval(long min, long max, long intercept)
    {
        return new Range(min + intercept, min, max - min);
    }

    public static Range Identity(long min, long max)
    {
        return FromInterval(min, max, intercept: 0);
    }
}
