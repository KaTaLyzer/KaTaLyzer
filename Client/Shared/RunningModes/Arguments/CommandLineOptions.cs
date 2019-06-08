using System.ComponentModel;
using CommandLine;

namespace Shared.RunningModes.Arguments
{
    public class CommandLineOptions
    {
        [Option('c', "config", Required = false, HelpText = "Sets configuration file")]
        [DefaultValue(null)]
        public string PathToConfig { get; set; }

        [Option('t', "template", Required = false, HelpText = "Creates default \"config.json\" file")]
        [DefaultValue(false)]
        public bool CreateConfigTemplate { get; set; }

        [Option('h', "help", Required = false, HelpText = "Prints help and exists")]
        [DefaultValue(false)]
        public bool Help { get; set; }

        [Option('b', "background", Required = false, HelpText = "Starts KaTaLyzer in background mode. You must specify config for this or default config is used")]
        [DefaultValue(false)]
        public bool RunInBackground { get; set; }
    }
}