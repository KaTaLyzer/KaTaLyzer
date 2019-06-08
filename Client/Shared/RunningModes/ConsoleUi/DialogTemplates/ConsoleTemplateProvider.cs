using System;
using System.Collections.Generic;

namespace Shared.RunningModes.ConsoleUi.DialogTemplates
{
    public static class ConsoleTemplateProvider
    {
        public static UserDialogResponse BuildYesNoDialog(string question, string positiveResponse,
            string negativeResponse, bool intercept)
        {
            Console.Write($"{question} [Y/N]? ");

            while (true)
            {
                var key = Console.ReadKey(intercept);

                if (key.Key == ConsoleKey.Y)
                {
                    Console.WriteLine(positiveResponse);
                    return UserDialogResponse.Positive;
                }

                if (key.Key == ConsoleKey.N)
                {
                    Console.WriteLine(negativeResponse);
                    return UserDialogResponse.Negative;
                }

                if (ConsoleKey.Escape == key.Key)
                {
                    return UserDialogResponse.NotAnswered;
                }
            }
        }

        public static string GetUserStringInput(string message, string defaultValue = null)
        {
            Console.Write(string.IsNullOrWhiteSpace(defaultValue)
                ? $"{message}:"
                : $"{message}(default \"{defaultValue}\"):");

            var result = Console.ReadLine();

            if (string.IsNullOrEmpty(result?.Trim()))
            {
                if (defaultValue != null)
                {
                    result = defaultValue;
                }
                else
                {
                    result = GetUserStringInput(message);
                }
            }            

            return result;
        }

        public static double GetUserDoubleInput(string message)
        {
            bool valid = false;

            double result = 0;

            while (!valid)
            {
                valid = double.TryParse(GetUserStringInput(message), out var interval);

                if (valid)
                {
                    result = interval;
                }
            }

            return result;
        }



        public static void BuildOptionsDialog(string optionsTitle, List<DialogActionOption> dialogOptions)
        {
            Console.WriteLine(optionsTitle);

            foreach (var dialogOption in dialogOptions)
                Console.WriteLine($"{dialogOptions.IndexOf(dialogOption) + 1}. {dialogOption.Message}");

            Console.WriteLine("(Use numbers to select from options)");

            var gotValidResult = false;

            while (!gotValidResult)
            {
                var key = Console.ReadKey(true);

                var validKey = int.TryParse(key.KeyChar.ToString(), out var index);

                if (validKey && index > 0 && index <= dialogOptions.Count)
                {
                    gotValidResult = true;

                    var dialogOption = dialogOptions[index - 1];

                    Console.WriteLine();

                    Console.WriteLine(dialogOption.Message);

                    dialogOption.Action.Invoke(null, EventArgs.Empty);
                }
            }
        }
    }
}