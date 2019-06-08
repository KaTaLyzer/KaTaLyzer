using System;
using System.IO;
using Shared.Logging;
using Shared.RunningModes.Arguments;

namespace Linux
{
    internal class Program
    {
        private static void Main(string[] arguments)
        {
            Directory.SetCurrentDirectory(AppDomain.CurrentDomain.BaseDirectory);

            AppDomain.CurrentDomain.UnhandledException += (sender, args) =>
            {
                KaTaLyzerLogger.Log(LogLevel.Fatal, (Exception)args.ExceptionObject).Wait();
            };

            new ArgumentsResolver().ResolveArguments(arguments, new LinuxPlatform());
        }
    }
}