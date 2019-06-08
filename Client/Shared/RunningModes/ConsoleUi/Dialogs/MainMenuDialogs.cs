using System;
using System.Collections.Generic;
using Shared.Configuration;
using Shared.Core.Capturing;
using Shared.RunningModes.ConsoleUi.DialogTemplates;
using SharpPcap.LibPcap;

namespace Shared.RunningModes.ConsoleUi.Dialogs
{
    public class MainMenuDialogs
    {
        public void RefreshRunningWorkerInfo(DateTime started, DateTime lastUpload, ulong count)
        {
            Console.Clear();
            Console.WriteLine($"Capturing started: {started}");
            Console.WriteLine($"Last persisting: {lastUpload}");
            Console.WriteLine($"Persist counter: {count}");
        }

        public void InvalidArgumentsWerePassed()
        {
            Console.WriteLine("Invalid arguments were passed to KaTaLyzer.");
        }

        public void ShowHelp()
        {
            Console.WriteLine("KaTaLyzer is tool used for monitoring network traffic that flows through network node");
            Console.WriteLine("");
            Console.WriteLine("Arguments you can pass to KaTaLyzer:");
            Console.WriteLine("");
            Console.WriteLine("\t-c --config=<filename> => Sets configuration file");
            Console.WriteLine("\t-t --template => Creates default \"config.json\" file");
            Console.WriteLine($"\t-b --background => Starts KaTaLyzer in background mode. You must specify config with this argument or default config \"{ConfigurationManager.DefaultConfigurationFilePath}\"will be used used");
            Console.WriteLine("\t-h --help => Prints help and exists");
            Console.WriteLine("Press escape to continue...");
            WaitUntilKeyIsPressed(ConsoleKey.Escape);
        }

        public void CapturingStarted()
        {
            Console.WriteLine("Capturing started");
        }

        public void ShowMainMenuOptions(List<PcapDevice> adapterProvider, Action startWorker)
        {
            bool exit = false;
            while (!exit)
            {
                Console.Clear();

                var startingDialogOptions = new List<DialogActionOption>
                {
                    new DialogActionOption
                    {
                        Action = (sender, args) =>
                        {
                            string path = ConsoleTemplateProvider.GetUserStringInput("Enter path to configuration",
                                ConfigurationManager.DefaultConfigurationFilePath);

                            ConfigurationManager.GetInstance().LoadConfiguration(path);
                        },
                        Message = "Load configuration"
                    },

                    new DialogActionOption
                    {
                        Action = (sender, args) =>
                        {
                            var configurationWizard = new ConfigurationWizardDialogs(adapterProvider);

                            var configuration = configurationWizard.CreateConfiguration();

                            var path = ConsoleTemplateProvider.GetUserStringInput("Enter path to store configuration",
                                ConfigurationManager.DefaultConfigurationFilePath);

                            ConfigurationManager.GetInstance()
                                .SaveConfiguration(configuration, path, false, FileExistsAction);
                        },
                        Message = "Create new configuration"
                    },

                    new DialogActionOption
                    {
                        Action = (sender, args) =>
                        {
                            if (ConfigurationManager.GetInstance().IsConfigLoaded())
                            {
                                startWorker.Invoke();
                            }
                        },
                        Message = ConfigurationManager.GetInstance().IsConfigLoaded()
                            ? $"Start capturing(current config={ConfigurationManager.GetInstance().GetPathToCurrentConfig()})"
                            : "[DISABLED] Start capturing - Missing config"
                    },

                    new DialogActionOption
                    {
                        Action = (sender, args) =>
                        {
                            ShowHelp();
                        },
                        Message = "Show help"
                    },

                    new DialogActionOption
                    {
                        Action = (sender, args) =>
                        {
                            exit = true;
                        },
                        Message = "Exit"
                    }
                };

                ConsoleTemplateProvider.BuildOptionsDialog("MENU", startingDialogOptions);
            }
        }

        private void FileExistsAction(Settings settings, string filename)
        {
            var startingDialogOptions = new List<DialogActionOption>
            {
                new DialogActionOption
                {
                    Action = (sender, args) =>
                    {
                        ConfigurationManager.GetInstance()
                            .SaveConfiguration(settings, filename, true, FileExistsAction);
                    },
                    Message = "Replace configuration"
                },

                new DialogActionOption
                {
                    Action = (sender, args) =>
                    {
                        var path = ConsoleTemplateProvider.GetUserStringInput("Enter path to store configuration",
                            ConfigurationManager.DefaultConfigurationFilePath);

                        ConfigurationManager.GetInstance().SaveConfiguration(settings, path, false, FileExistsAction);
                    },
                    Message = "Change filename"
                },

                new DialogActionOption
                {
                    Action = (sender, args) => { },
                    Message = "Exit"
                }
            };

            ConsoleTemplateProvider.BuildOptionsDialog($"Configuration \"{filename}\" exists. Would you like to:",
                startingDialogOptions);
        }

        public void CapturingStopped()
        {
            Console.WriteLine("Capturing stopped");
        }

        public void WaitUntilKeyIsPressed(ConsoleKey key)
        {
            while (Console.ReadKey(true).Key != key)
            {

            }
        }

        public void ErrorOccuredDuringCapturing()
        {
            Console.Clear();
            Console.WriteLine("An error occured.");
            Console.WriteLine("Possible reasons:");
            Console.WriteLine("\tOne or more monitored network adapters are disabled and can not be monitored");
            Console.WriteLine("\tServer is offline or computer has no internet connection");
            Console.WriteLine("\tKaTaLyzer was not started with administrator/root rights");
            Console.WriteLine("\tYou are using old version");
            Console.WriteLine($"\tIncorrect configuration. Current config\"{ConfigurationManager.GetInstance().GetPathToCurrentConfig()}\". Current server address \"{ConfigurationManager.GetInstance().GetServerAddress()}\"");
            Console.WriteLine("");
            Console.WriteLine("Check logs for more information");
            Console.WriteLine("");
        }
    }
}
