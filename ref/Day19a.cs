// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Text;

namespace Day19;

internal enum Property
{
    X = 0,
    M = 1,
    A = 2,
    S = 3,
}

internal interface IExpression
{
    bool Evaluate(Dynamic value, FunctionDictionary expression);
}

internal sealed class Dynamic
{
    private readonly int[] _properties = new int[Enum.GetValues<Property>().Length];

    public int this[Property property]
    {
        get
        {
            return _properties[(int)property];
        }
        set
        {
            _properties[(int)property] = value;
        }
    }
}

internal sealed class AcceptAction : IExpression
{
    public override string ToString()
    {
        return "A";
    }

    public bool Evaluate(Dynamic value, FunctionDictionary expression)
    {
        return true;
    }
}

internal sealed class RejectAction : IExpression
{
    public override string ToString()
    {
        return "R";
    }

    public bool Evaluate(Dynamic value, FunctionDictionary expression)
    {
        return false;
    }
}

internal sealed class JumpAction : IExpression
{
    public JumpAction(string destination)
    {
        Destination = destination;
    }

    public string Destination { get; set; }

    public override string ToString()
    {
        return Destination;
    }

    public bool Evaluate(Dynamic value, FunctionDictionary expression)
    {
        return expression[Destination].Evaluate(value, expression);
    }
}

internal sealed class LessThanCondition : IExpression
{
    public LessThanCondition(Property identifier, int comparand)
    {
        Identifier = identifier;
        Comparand = comparand;
    }

    public Property Identifier { get; }
    public int Comparand { get; }

    public override string ToString()
    {
        return $"{Identifier.ToString().ToLowerInvariant()}<{Comparand}";
    }

    public bool Evaluate(Dynamic value, FunctionDictionary expression)
    {
        return value[Identifier] < Comparand;
    }
}

internal sealed class GreaterThanCondition : IExpression
{
    public GreaterThanCondition(Property identiifer, int comparand)
    {
        Identifier = identiifer;
        Comparand = comparand;
    }

    public Property Identifier { get; }
    public int Comparand { get; }

    public override string ToString()
    {
        return $"{Identifier.ToString().ToLowerInvariant()}>{Comparand}";
    }

    public bool Evaluate(Dynamic value, FunctionDictionary expression)
    {
        return value[Identifier] > Comparand;
    }
}

internal sealed class Range
{
    public Range(IExpression condition, IExpression action)
    {
        Condition = condition;
        Action = action;
    }

    public IExpression Condition { get; }
    public IExpression Action { get; }

    public override string ToString()
    {
        return $"{Condition}:{Action}";
    }
}

internal sealed class Function
{
    private readonly List<Range> _ranges = new List<Range>();

    public ICollection<Range> Ranges
    {
        get
        {
            return _ranges;
        }
    }

    public void Write(StringBuilder output)
    {
        output.AppendJoin(',', _ranges);
    }

    public bool Evaluate(Dynamic value, FunctionDictionary expression)
    {
        foreach (Range range in _ranges)
        {
            if (range.Condition.Evaluate(value, expression))
            {
                return range.Action.Evaluate(value, expression);
            }
        }

        return false;
    }
}

internal sealed class FunctionDictionary
{
    private readonly Dictionary<string, Function> _functions = new Dictionary<string, Function>();

    public Function this[string name]
    {
        get
        {
            return _functions[name];
        }
        set
        {
            _functions[name] = value;
        }
    }

    public void Write(StringBuilder output)
    {
        foreach (KeyValuePair<string, Function> function in _functions)
        {
            output
                .Append(function.Key)
                .Append('{');

            function.Value.Write(output);

            output.AppendLine("}");
        }
    }

    public override string ToString()
    {
        StringBuilder result = new StringBuilder();

        Write(result);

        return result.ToString();
    }
}

internal sealed class Parser
{
    private string _tokens = string.Empty;
    private int _index = 0;

    private char Peek()
    {
        return _tokens[_index];
    }

    private bool TryPeekAhead(out char result)
    {
        if (_index + 1 >= _tokens.Length)
        {
            result = '\0';

            return false;
        }

        result = _tokens[_index + 1];

        return true;
    }

    private char Pop()
    {
        char result = _tokens[_index];

        _index++;

        return result;
    }

    public void SetTokens(string tokens)
    {
        _tokens = tokens;
        _index = 0;
    }

    private bool ParseKey([MaybeNullWhen(false)] out string result)
    {
        StringBuilder builder = new StringBuilder();

        while (char.IsLower(Peek()))
        {
            builder.Append(Pop());
        }

        result = builder.ToString();

        return true;
    }

    private static Property GetProperty(char token)
    {
        switch (token)
        {
            case 'a': return Property.A;
            case 'm': return Property.M;
            case 's': return Property.S;
            case 'x': return Property.X;
            default: throw new ArgumentException(null, nameof(token));
        }
    }

    private bool ParseAction([MaybeNullWhen(false)] out IExpression result)
    {
        switch (Peek())
        {
            case 'A':
                Pop();

                result = new AcceptAction();
                return true;

            case 'R':
                Pop();

                result = new RejectAction();
                return true;

            default:
                if (!ParseKey(out string? label))
                {
                    result = null;
                    return false;
                }

                result = new JumpAction(label);
                return true;
        }
    }

    private bool ParseComparand(out int comparand)
    {
        comparand = 0;

        while (char.IsDigit(Peek()))
        {
            comparand = (comparand * 10) + (Pop() - '0');
        }

        return comparand > 0;
    }

    private bool ParseRange([MaybeNullWhen(false)] out Range result)
    {
        IExpression? action;

        if (TryPeekAhead(out char ahead))
        {
            switch (ahead)
            {
                case '<':
                    char first = Pop();

                    Pop();

                    if (!ParseComparand(out int comparand) || Pop() != ':' || !ParseAction(out action))
                    {
                        result = null;

                        return false;
                    }

                    result = new Range(new LessThanCondition(GetProperty(first), comparand), action);
                    return true;

                case '>':
                    first = Pop();

                    Pop();

                    if (!ParseComparand(out comparand) || Pop() != ':' || !ParseAction(out action))
                    {
                        result = null;

                        return false;
                    }

                    result = new Range(new GreaterThanCondition(GetProperty(first), comparand), action);
                    return true;
            }
        }

        if (!ParseAction(out action))
        {
            result = null;

            return false;
        }

        result = new Range(new AcceptAction(), action);

        return true;
    }

    public bool ParseFunction(
        [MaybeNullWhen(false)] out string key,
        [MaybeNullWhen(false)] out Function result)
    {
        if (!ParseKey(out key) || Pop() != '{')
        {
            result = null;

            return false;
        }

        result = new Function();

        do
        {
            if (!ParseRange(out Range? range))
            {
                return false;
            }

            result.Ranges.Add(range);
        }
        while (Pop() == ',');

        return true;
    }
}

internal static class Program
{
    private static void Main()
    {
        using StreamReader reader = File.OpenText("mini.txt");

        Stopwatch stopwatch = Stopwatch.StartNew();

        int result = Run(reader);

        stopwatch.Stop();
        Console.WriteLine("19a {0} {1}", result, stopwatch.Elapsed.TotalSeconds);
    }

    private static int Run(StreamReader reader)
    {
        string? line;
        Parser parser = new Parser();
        FunctionDictionary dictionary = new FunctionDictionary();

        while ((line = reader.ReadLine()) != null)
        {
            if (line.Length == 0)
            {
                break;
            }

            parser.SetTokens(line);

            if (!parser.ParseFunction(out string? name, out Function? function))
            {
                throw new FormatException();
            }

            dictionary[name] = function;
        }

        int sum = 0;

        while ((line = reader.ReadLine()) != null) 
        {
            Dynamic dynamic = new Dynamic();
            string[] segments = line.Substring(0, line.Length - 1).Split(',');
            int localSum = 0;

            foreach (Property property in Enum.GetValues<Property>())
            {
                string segment = segments[(int)property];
                int index = segment.IndexOf('=');

                if (index == -1)
                {
                    throw new FormatException();
                }

                int number = int.Parse(segment.Substring(index + 1)); 

                dynamic[property] = number;
                localSum += number;
            }

            if (dictionary["in"].Evaluate(dynamic, dictionary))
            {
                sum += localSum;
            }
        }

        return sum;
    }
}
