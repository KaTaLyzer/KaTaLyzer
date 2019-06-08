using System;

namespace Shared.RunningModes.ConsoleUi.DialogTemplates
{
    public class DialogActionOption
    {
        public EventHandler Action;
        public string Message { get; set; }
    }
}