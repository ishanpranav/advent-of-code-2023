using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Text;

namespace Day19;

internal enum Identifier
{
    X = 0,
    M = 1,
    A = 2,
    S = 3,
}

internal interface IExpression
{
    bool Evaluate(Dynamic value, Expression expression);
}

internal sealed class Dynamic
{
    private readonly int[] _properties = new int[Enum.GetValues<Identifier>().Length];

    public int this[Identifier property]
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

    public bool Evaluate(Dynamic value, Expression expression)
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

    public bool Evaluate(Dynamic value, Expression expression)
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

    public bool Evaluate(Dynamic value, Expression expression)
    {
        return expression[Destination].Evaluate(value, expression);
    }
}

internal sealed class LessThanCondition : IExpression
{
    public LessThanCondition(Identifier identifier, int comparand)
    {
        Identifier = identifier;
        Comparand = comparand;
    }

    public Identifier Identifier { get; }
    public int Comparand { get; }

    public override string ToString()
    {
        return $"{Identifier.ToString().ToLowerInvariant()}<{Comparand}";
    }

    public bool Evaluate(Dynamic value, Expression expression)
    {
        return value[Identifier] < Comparand;
    }
}

internal sealed class GreaterThanCondition : IExpression
{
    public GreaterThanCondition(Identifier identiifer, int comparand)
    {
        Identifier = identiifer;
        Comparand = comparand;
    }

    public Identifier Identifier { get; }
    public int Comparand { get; }

    public override string ToString()
    {
        return $"{Identifier.ToString().ToLowerInvariant()}>{Comparand}";
    }

    public bool Evaluate(Dynamic value, Expression expression)
    {
        return value[Identifier] > Comparand;
    }
}

internal sealed class Rule
{
    public Rule(IExpression condition, IExpression action)
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

internal sealed class RuleSet
{
    private readonly List<Rule> _rules = new List<Rule>();

    public ICollection<Rule> Rules
    {
        get
        {
            return _rules;
        }
    }

    public void Write(StringBuilder output)
    {
        output.AppendJoin(',', _rules);
    }

    public bool Evaluate(Dynamic value, Expression expression)
    {
        foreach (Rule rule in _rules)
        {
            if (rule.Condition.Evaluate(value, expression))
            {
                return rule.Action.Evaluate(value, expression);
            }
        }

        return false;
    }
}

internal sealed class Expression
{
    private readonly Dictionary<string, RuleSet> _ruleSets = new Dictionary<string, RuleSet>();

    public RuleSet this[string name]
    {
        get
        {
            return _ruleSets[name];
        }
        set
        {
            _ruleSets[name] = value;
        }
    }

    public void Write(StringBuilder output)
    {
        foreach (KeyValuePair<string, RuleSet> ruleSet in _ruleSets)
        {
            output
                .Append(ruleSet.Key)
                .Append('{');

            ruleSet.Value.Write(output);

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

    private bool ParseLabel([MaybeNullWhen(false)] out string result)
    {
        StringBuilder builder = new StringBuilder();

        while (char.IsLower(Peek()))
        {
            builder.Append(Pop());
        }

        result = builder.ToString();

        return true;
    }

    private static Identifier GetIdentifier(char token)
    {
        switch (token)
        {
            case 'a': return Identifier.A;
            case 'm': return Identifier.M;
            case 's': return Identifier.S;
            case 'x': return Identifier.X;
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
                if (!ParseLabel(out string? label))
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

    private bool ParseRule([MaybeNullWhen(false)] out Rule result)
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

                    result = new Rule(new LessThanCondition(GetIdentifier(first), comparand), action);
                    return true;

                case '>':
                    first = Pop();

                    Pop();

                    if (!ParseComparand(out comparand) || Pop() != ':' || !ParseAction(out action))
                    {
                        result = null;

                        return false;
                    }

                    result = new Rule(new GreaterThanCondition(GetIdentifier(first), comparand), action);
                    return true;
            }
        }

        if (!ParseAction(out action))
        {
            result = null;

            return false;
        }

        result = new Rule(new AcceptAction(), action);

        return true;
    }

    public bool ParseRuleSet(
        [MaybeNullWhen(false)] out string name,
        [MaybeNullWhen(false)] out RuleSet ruleSet)
    {
        if (!ParseLabel(out name))
        {
            ruleSet = null;

            return false;
        }

        if (Pop() != '{')
        {
            ruleSet = null;

            return false;
        }

        ruleSet = new RuleSet();

        do
        {
            if (!ParseRule(out Rule? rule))
            {
                return false;
            }

            ruleSet.Rules.Add(rule);
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
        Expression expression = new Expression();

        while ((line = reader.ReadLine()) != null)
        {
            if (line.Length == 0)
            {
                break;
            }

            parser.SetTokens(line);

            if (!parser.ParseRuleSet(out string? name, out RuleSet? ruleSet))
            {
                throw new FormatException();
            }

            expression[name] = ruleSet;
        }

        int sum = 0;

        while ((line = reader.ReadLine()) != null) 
        {
            Dynamic dynamic = new Dynamic();
            string[] segments = line.Substring(0, line.Length - 1).Split(',');
            int localSum = 0;

            foreach (Identifier identifier in Enum.GetValues<Identifier>())
            {
                string segment = segments[(int)identifier];
                int index = segment.IndexOf('=');

                if (index == -1)
                {
                    throw new FormatException();
                }

                int number = int.Parse(segment.Substring(index + 1)); 

                dynamic[identifier] = number;
                localSum += number;
            }

            if (expression["in"].Evaluate(dynamic, expression))
            {
                sum += localSum;
            }
        }

        Console.WriteLine(expression.ToString());

        return sum;
    }
}
