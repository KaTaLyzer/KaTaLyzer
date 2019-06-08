using System;
using System.Collections.Generic;
using System.Linq;
using CommandLine;
using Shared.Configuration;
using Shared.Core.Protocols;
using Shared.Logging;

namespace Shared.RunningModes.Arguments
{
    public class ArgumentsResolver
    {
        public void ResolveArguments(string[] commandLineArguments, PlatformDependencies platform)
        {
            KaTaLyzerLogger.Log(LogLevel.Trace, "KaTaLyzer started", new KeyValuePair<string, object>("Arguments", commandLineArguments)).Wait();

            if (commandLineArguments.Length == 0)
            {
                ConsoleUi.ConsoleUi consoleUi = new ConsoleUi.ConsoleUi(platform);
                consoleUi.ShowMainMenu();
            }
            else
            {
                Parser parser = new Parser(settings => { settings.AutoHelp = false; });

                parser.ParseArguments<CommandLineOptions>(commandLineArguments).WithParsed(options =>
                {
                    if (options.Help)
                    {
                        ConsoleUi.ConsoleUi consoleUi = new ConsoleUi.ConsoleUi(platform);
                        consoleUi.ShowHelp();
                        Environment.Exit(0);
                    }

                    if (options.CreateConfigTemplate)
                    {
                        Settings templateSetting = new Settings()
                        {
                            DeviceName = "<DeviceName>",
                            ServerAddress = "<http://localhost:31311>",
                            MonitoredNetworkAdapters = platform.GetCurrentDevices().Select(x => new NetworkAdapterConfiguration {CustomName = x.Interface.FriendlyName, InterfaceFriendlyName = x.Interface.FriendlyName}).ToList(),
                            UploadInterval = 60,
                            MonitoredProtocols = ProtocolsLists.AllProtocols.ToDictionary(x => x, x => true)
                        };

                        ConfigurationManager.GetInstance().SaveConfiguration(templateSetting, ConfigurationManager.DefaultConfigurationFilePath, true, null);

                        Console.WriteLine($"Configuration \"{ConfigurationManager.DefaultConfigurationFilePath}\" created successful");

                        Environment.Exit(0);
                    }
                    else
                    {
                        if (!string.IsNullOrEmpty(options.PathToConfig))
                        {
                            ConfigurationManager.GetInstance().LoadConfiguration(options.PathToConfig);
                        }
                    }

                    if (options.RunInBackground)
                    {
                        platform.RunInBackground();
                    }
                    else
                    {
                        ConsoleUi.ConsoleUi consoleUi = new ConsoleUi.ConsoleUi(platform);
                        consoleUi.ShowMainMenu();
                    }
                }).WithNotParsed(errors =>
                {
                    ConsoleUi.ConsoleUi consoleUi = new ConsoleUi.ConsoleUi(platform);
                    consoleUi.InvalidArgumentsWerePassed();
                    consoleUi.ShowHelp();
                });
            }
        }
    }
}
