// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Almost-optimized function composition and simplification approach

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text;

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

        Console.OutputEncoding = Encoding.Unicode;

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
        List<Range> seeds = new List<Range>();

        for (int i = 1; i < tokens.Length - 1; i += 2)
        {
            long offset = long.Parse(tokens[i], CultureInfo.InvariantCulture);

            Range range = new Range(
                offset,
                offset,
                long.Parse(tokens[i + 1], CultureInfo.InvariantCulture));

            seeds.Add(range);
        }

        Function? current = null;
        Function? composite = null;
        List<Function> functions = new List<Function>();

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

        foreach (Range seed in seeds)
        {
            long seedMin = seed.Min;
            long seedMax = seed.Max;

            foreach (Range range in composite.Ranges)
            {
                if (range.Min < seedMin || seedMax < range.Max)
                {
                    continue;
                }

                long result = Math.Max(range.Min, seedMin) + range.Intercept;

                if (result < min)
                {
                    min = result;
                }
            }
        }

        Console.WriteLine(@"\[ f(x) = \begin{cases}");

        foreach (Range range in composite.Ranges)
        {
            Console.Write("x");
            if (range.Intercept != 0)
            {
                Console.Write("{0:+#;-#}", range.Intercept);
            }
            Console.Write(@"&{0}\leq", range.Min);
            if (range.Max == long.MaxValue)
            {
                Console.Write(@"\infty");
            }
            else
            {
                Console.Write(" {0}", range.Max);
            }
            Console.WriteLine(@"\\");
        }
        Console.WriteLine(@"\end{cases}\]");
        stopwatch.Stop();

        elapsed = stopwatch.Elapsed;
    }
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
        long min = first.Min;

        if (min != 0)
        {
            _ranges.Add(Range.Identity(min: 0, min));
        }

        _ranges.Add(first);

        for (int i = 1; i < view.Length; i++)
        {
            Range current = view[i];
            Range previous = view[i - 1];

            long gap = current.Min - previous.Max;

            if (gap > 1)
            {
                _ranges.Add(Range.Identity(previous.Max + 1, current.Max));
            }

            _ranges.Add(current);
        }

        Range last = _ranges[_ranges.Count - 1];

        if (last.Max < long.MaxValue)
        {
            _ranges.Add(Range.Identity(last.Max + 1, long.MaxValue));
        }
    }

    public static Function Compose(Function f, Function g)
    {
        Function result = new Function();

        foreach (Range a in f._ranges)
        {
            foreach (Range b in g._ranges)
            {
                long bMin = b.Min - a.Intercept;
                long bMax = b.Max - a.Intercept;

                if (a.Max < bMin || bMax < a.Min)
                {
                    continue;
                }

                result.AddRange(Range.FromInterval(
                    Math.Max(a.Min, bMin),
                    Math.Min(a.Max, bMax),
                    a.Intercept + b.Intercept));
            }
        }

        return result;
    }

    public void ClearRanges()
    {
        _ranges.Clear();
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

    public override string ToString()
    {
        return string.Join(", ", _ranges);
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

    public long Min
    {
        get
        {
            return SourceOffset;
        }
    }

    public long Max
    {
        get
        {
            return SourceOffset + Length;
        }
    }

    public long Intercept
    {
        get
        {
            return DestinationOffset - SourceOffset;
        }
    }

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
        if (Max == long.MaxValue)
        {
            return $"x in [{Min}, \u221e)";
        }

        if (Intercept == 0)
        {
            return $"x in [{Min}, {Max})";
        }

        return $"x {Intercept:+ #;- #} in [{Min}, {Max})";
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
