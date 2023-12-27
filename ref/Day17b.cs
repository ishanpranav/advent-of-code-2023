using System.Diagnostics;

namespace Advent;

internal enum Direction
{
    None,
    Vertical,
    Horizontal
}

internal readonly struct Coordinate
{
    public Coordinate(int i, int j, Direction forbidden)
    {
        I = i;
        J = j;
        Forbidden = forbidden;
    }

    public int I { get; }
    public int J { get; }
    public Direction Forbidden { get; }
}

internal sealed class State
{
    public State(int priority)
    {
        Priority = priority;
        Hi = 9999;
        Lo = 9999;
        Left = 9999;
        Right = 9999;
    }

    public int Priority { get; }
    public int Hi { get; set; }
    public int Lo { get; set; }
    public int Left { get; set; }
    public int Right { get; set; }
}

internal sealed class StateMatrix
{
    private readonly State[,] _items = new State[150, 150];

    public StateMatrix(int columns)
    {
        Columns = columns;
    }

    public State this[int i, int j]
    {
        get
        {
            return _items[i, j];
        }
        set
        {
            _items[i, j] = value;
        }
    }

    public State this[Coordinate coordinate]
    {
        get
        {
            return this[coordinate.I, coordinate.J];
        }
        set
        {
            this[coordinate.I, coordinate.J] = value;
        }
    }

    public int Rows { get; private set; }
    public int Columns { get; private set; }

    public void AddRow()
    {
        Rows++;
    }
}

internal static class Program
{
    public const int Min = 4;
    public const int Max = 10;

    private static void Main()
    {
        using StreamReader reader = File.OpenText("mini.txt");

        Stopwatch stopwatch = Stopwatch.StartNew();

        int result = Run(reader);

        stopwatch.Stop();
        Console.WriteLine("17a {0} {1}", result, stopwatch.Elapsed.TotalSeconds);
    }

    private static void ScanHi(StateMatrix matrix, Coordinate current, List<Coordinate> next)
    {
        State initialState = matrix[current];

        int j = current.J;
        int priority = Math.Min(initialState.Left, initialState.Right);

        for (int k = 1; k <= Max; k++)
        {
            int i = current.I - k;

            if (i < 0)
            {
                return;
            }

            State currentState = matrix[i, j];

            priority += currentState.Priority;

            if (k < Min || currentState.Lo <= priority)
            {
                continue;
            }

            currentState.Lo = priority;

            next.Add(new Coordinate(i, j, Direction.Vertical));
        }
    }

    private static void ScanLo(StateMatrix matrix, Coordinate current, List<Coordinate> next)
    {
        State initialState = matrix[current];

        int j = current.J;
        int priority = Math.Min(initialState.Left, initialState.Right);

        for (int k = 1; k <= Max; k++)
        {
            int i = current.I + k;

            if (i >= matrix.Rows)
            {
                return;
            }

            State currentState = matrix[i, j];

            priority += currentState.Priority;

            if (k < Min || currentState.Hi <= priority)
            {
                continue;
            }

            currentState.Hi = priority;

            next.Add(new Coordinate(i, j, Direction.Vertical));
        }
    }

    private static void ScanLeft(StateMatrix matrix, Coordinate current, List<Coordinate> next)
    {
        State initialState = matrix[current];

        int i = current.I;
        int priority = Math.Min(initialState.Hi, initialState.Lo);

        for (int k = 1; k <= Max; k++)
        {
            int j = current.J - k;

            if (j < 0)
            {
                return;
            }

            State currentState = matrix[i, j];

            priority += currentState.Priority;

            if (k < Min || currentState.Right <= priority)
            {
                continue;
            }

            currentState.Right = priority;

            next.Add(new Coordinate(i, j, Direction.Horizontal));
        }
    }

    private static void ScanRight(StateMatrix matrix, Coordinate current, List<Coordinate> next)
    {
        State initialState = matrix[current];

        int i = current.I;
        int priority = Math.Min(initialState.Hi, initialState.Lo);

        for (int k = 1; k <= Max; k++)
        {
            int j = current.J + k;

            if (j >= matrix.Columns)
            {
                return;
            }

            State currentState = matrix[i, j];

            priority += currentState.Priority;

            if (k < Min || currentState.Left <= priority)
            {
                continue;
            }

            currentState.Left = priority;

            next.Add(new Coordinate(i, j, Direction.Horizontal));
        }
    }

    private static int Run(StreamReader reader)
    {
        string? line = reader.ReadLine();

        if (line == null)
        {
            throw new FormatException();
        }

        StateMatrix matrix = new StateMatrix(line.Length);

        do
        {
            matrix.AddRow();

            for (int j = 0; j < line.Length; j++)
            {
                matrix[matrix.Rows - 1, j] = new State(line[j] - '0');
            }
        }
        while ((line = reader.ReadLine()) != null);

        State initial = matrix[0, 0];

        initial.Hi = 0;
        initial.Lo = 0;
        initial.Left = 0;
        initial.Right = 0;

        List<Coordinate> nextList = new List<Coordinate>
        {
            new Coordinate(0, 0, Direction.None)
        };

        while (nextList.Count > 0)
        {
            List<Coordinate> currentList = nextList;

            nextList = new List<Coordinate>();

            foreach (Coordinate current in currentList)
            {
                if (current.Forbidden != Direction.Vertical)
                {
                    ScanHi(matrix, current, nextList);
                    ScanLo(matrix, current, nextList);
                }

                if (current.Forbidden != Direction.Horizontal)
                {
                    ScanLeft(matrix, current, nextList);
                    ScanRight(matrix, current, nextList);
                }
            }
        }

        State target = matrix[matrix.Rows - 1, matrix.Columns - 1];
        int min = target.Hi;

        if (target.Lo < min)
        {
            min = target.Lo;
        }

        if (target.Left < min)
        {
            min = target.Left;
        }

        if (target.Right < min)
        {
            min = target.Right;
        }

        return min;
    }
}
