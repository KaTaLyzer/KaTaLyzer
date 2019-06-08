using System;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Shared.Logging;
using Shared.RunningModes.Arguments;
using SharpPcap;

namespace Windows
{
    internal class Program
    {
        private static void Main(string[] arguments)
        {
            Directory.SetCurrentDirectory(AppDomain.CurrentDomain.BaseDirectory);

            KaTaLyzerLogger.Log(LogLevel.Info, string.Join(",", arguments)).Wait();

            AppDomain.CurrentDomain.UnhandledException += (sender, args) =>
            {
                KaTaLyzerLogger.Log(LogLevel.Fatal, (Exception) args.ExceptionObject).Wait();
            };

            new ArgumentsResolver().ResolveArguments(arguments, new WindowsPlatform());
        }

        private static void SendTestingPackets()
        {
            new WindowsPlatform().GetCurrentDevices().ForEach(device =>
            {
                var x =
                    "C5 00 00 00 82 C4 00 12 1E F2 61 3D 81 00 00 64 86 DD 60 00 00 00 00 8B 04 F6 24 02 F0 00 00 01 8E 01 00 00 00 00 00 00 55 55 26 07 FC D0 01 00 23 00 00 00 00 00 B1 08 2A 6B 45 00 00 8B 8C AF 00 00 40 2F 75 FE 10 00 00 C8 C0 34 A6 9A 30 81 88 0B 00 67 17 80 00 06 8F B1 00 08 3A 76 FF 03 00 21 45 00 00 63 00 00 40 00 3C 11 56 67 AC 10 2C 03 08 08 08 08 9F 40 00 35 00 4F 2D 23 A6 2C 01 00 00 01 00 00 00 00 00 00 35 78 71 74 2D 64 65 74 65 63 74 2D 6D 6F 64 65 32 2D 39 37 37 31 32 65 38 38 2D 31 36 37 61 2D 34 35 62 39 2D 39 33 65 65 2D 39 31 33 31 34 30 65 37 36 36 37 38 00 00 1C 00 01";

                byte[] ss = x.Split(new[] { ' ' }).Select(y => byte.Parse(int.Parse(y, System.Globalization.NumberStyles.HexNumber).ToString())).ToArray();

                device.Open(DeviceMode.Normal);

                Task.Run(() =>
                {
                    while (true)
                    {
                        device.SendPacket(ss);
                        Thread.Sleep(10);
                    }
                });
            });
        }
    }
}