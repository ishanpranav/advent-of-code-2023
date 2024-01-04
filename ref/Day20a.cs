// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace Day20;

internal abstract class Module
{
    protected Module(string name)
    {
        Name = name;
    }

    public string Name { get; }
    public ICollection<string> Targets { get; } = new List<string>();

    public void Send(bool pulse, Queue<Message> queue)
    {
        foreach (string target in Targets)
        {
            queue.Enqueue(new Message(Name, target, pulse));
        }
    }

    public abstract void Respond(Message message, Queue<Message> queue);
}

internal sealed class FlipFlopModule : Module
{
    public FlipFlopModule(string name) : base(name) { }

    public bool Pulse { get; set; }

    public override void Respond(Message message, Queue<Message> queue)
    {
        if (message.Pulse)
        {
            return;
        }

        Pulse = !Pulse;

        Send(Pulse, queue);
    }
}

internal sealed class ConjunctionModule : Module
{
    public ConjunctionModule(string name) : base(name) { }

    public IDictionary<string, bool> Pulses { get; } = new Dictionary<string, bool>();

    public override void Respond(Message message, Queue<Message> queue)
    {
        Pulses[message.Source] = message.Pulse;

        bool result = false;

        foreach (bool pulse in Pulses.Values)
        {
            if (!pulse)
            {
                result = true;

                break;
            }
        }

        Send(result, queue);
    }
}

internal sealed class Message
{
    public Message(string source, string target, bool pulse)
    {
        Source = source;
        Target = target;
        Pulse = pulse;
    }

    public string Source { get; }
    public string Target { get; }
    public bool Pulse { get; }
}

internal static class Program
{
    private static void Main()
    {
        using StreamReader reader = File.OpenText("mini.txt");

        Stopwatch stopwatch = Stopwatch.StartNew();
        int result = Run(reader);

        stopwatch.Stop();
        Console.WriteLine("20a {0} {1}", result, stopwatch.Elapsed.TotalSeconds);
    }

    private static int Run(StreamReader reader)
    {
        string? line;
        Dictionary<string, Module> modules = new Dictionary<string, Module>();

        while ((line = reader.ReadLine()) != null)
        {
            int mid = line.IndexOf(" -> ");

            if (mid == -1)
            {
                throw new FormatException();
            }

            Module module;

            if (line[0] == '%')
            {
                module = new FlipFlopModule(line.Substring(1, mid - 1));
            }
            else
            {   
                module = new ConjunctionModule(line.Substring(1, mid - 1));
            }

            modules.Add(module.Name, module);

            foreach (string target in line.Substring(mid + 4).Split(", "))
            {
                module.Targets.Add(target);
            }
        }

        foreach (KeyValuePair<string, Module> module in modules)
        {
            foreach (string target in module.Value.Targets)
            {
                if (modules.TryGetValue(target, out Module? targetModule) && targetModule is ConjunctionModule conjunctionModule)
                {
                    conjunctionModule.Pulses[module.Key] = false;
                }
            }
        }

        Dictionary<bool, int> counts = new Dictionary<bool, int>()
        {
            [true] = 0,
            [false] = 1000
        };
        Queue<Message> queue = new Queue<Message>();

        for (int i = 0; i < 1000; i++)
        {
            modules["roadcaster"].Send(false, queue);

            while (queue.TryDequeue(out Message? current))
            {
                counts[current.Pulse]++;

                if (!modules.TryGetValue(current.Target, out Module? targetModule))
                {
                    continue;
                }

                targetModule.Respond(current, queue);
            }

            queue.Clear();
        }

        return counts[true] * counts[false];
    }
}
