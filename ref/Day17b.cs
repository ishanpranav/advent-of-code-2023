using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace Day17;

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
        Obstacle = forbidden;
    }

    public int I { get; }
    public int J { get; }
    public Direction Obstacle { get; }
}

internal sealed class State
{
    public State(int priority)
    {
        Priority = priority;
        Hi = int.MaxValue / 2;
        Lo = int.MaxValue / 2;
        Left = int.MaxValue / 2;
        Right = int.MaxValue / 2;
    }

    public int Priority { get; }
    public int Hi { get; set; }
    public int Lo { get; set; }
    public int Left { get; set; }
    public int Right { get; set; }
}

internal sealed class StateMatrix
{
    private readonly State[,] _items = new State[256, 256];

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
    public const int Max = 11;

    private static void Main()
    {
        using StreamReader reader = File.OpenText("mini.txt");

        Stopwatch stopwatch = Stopwatch.StartNew();

        int result = Run(reader);

        stopwatch.Stop();
        Console.WriteLine("17a {0} {1}", result, stopwatch.Elapsed.TotalSeconds);
    }

    private static void ScanHi(StateMatrix matrix, Coordinate current, int priority, PriorityQueue<Coordinate, int> coordinates)
    {
        int j = current.J;

        for (int k = 1; k < Max; k++)
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

            coordinates.Enqueue(new Coordinate(i, j, Direction.Vertical), priority);
        }
    }

    private static void ScanLo(StateMatrix matrix, Coordinate current, int priority, PriorityQueue<Coordinate, int> coordinates)
    {
        int j = current.J;

        for (int k = 1; k < Max; k++)
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

            coordinates.Enqueue(new Coordinate(i, j, Direction.Vertical), priority);
        }
    }

    private static void ScanLeft(StateMatrix matrix, Coordinate current, int priority, PriorityQueue<Coordinate, int> coordinates)
    {
        int i = current.I;
        
        for (int k = 1; k < Max; k++)
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

            coordinates.Enqueue(new Coordinate(i, j, Direction.Horizontal), priority);
        }
    }

    private static void ScanRight(StateMatrix matrix, Coordinate current, int priority, PriorityQueue<Coordinate, int> coordinates)
    {
        int i = current.I;
        
        for (int k = 1; k < Max; k++)
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

            coordinates.Enqueue(new Coordinate(i, j, Direction.Horizontal), priority);
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
                matrix[matrix.Rows - 1, j] = new State((int)(line[j] - '0'));
            }
        }
        while ((line = reader.ReadLine()) != null);

        State initialState = matrix[0, 0];

        initialState.Hi = 0;
        initialState.Lo = 0;
        initialState.Left = 0;
        initialState.Right = 0;

        PriorityQueue<Coordinate, int> queue = new PriorityQueue<Coordinate, int>();

        queue.Enqueue(new Coordinate(0, 0, Direction.None), 0);

        while (queue.TryDequeue(out Coordinate current, out _))
        {
            if (current.Obstacle != Direction.Vertical)
            {
                State currentState = matrix[current];
                int priority = Math.Min(currentState.Left, currentState.Right);

                ScanHi(matrix, current, priority, queue);
                ScanLo(matrix, current, priority, queue);
            }

            if (current.Obstacle != Direction.Horizontal)
            {
                State currentState = matrix[current];
                int priority = Math.Min(currentState.Hi, currentState.Lo);

                ScanLeft(matrix, current, priority, queue);
                ScanRight(matrix, current, priority, queue);
            }
        }

        State target = matrix[matrix.Rows - 1, matrix.Columns - 1];
        int min = Math.Min(target.Hi, target.Lo);

        min = Math.Min(min, target.Left);
        min = Math.Min(min, target.Right);

        return min;
    }
}
