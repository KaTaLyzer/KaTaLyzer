using System;
using System.Collections.Generic;
using Shared.Configuration;
using Shared.Core.Protocols;
using Shared.RunningModes.ConsoleUi.DialogTemplates;
using SharpPcap.LibPcap;

namespace Shared.RunningModes.ConsoleUi.Dialogs
{
    public class ConfigurationWizardDialogs
    {
        private readonly List<PcapDevice> _pcapDevices;
        private readonly Settings _settings = new Settings();

        public ConfigurationWizardDialogs(List<PcapDevice> pcapDevices)
        {
            _pcapDevices = pcapDevices;
        }

        public Settings CreateConfiguration()
        {
            Console.WriteLine("UPLOAD SETTINGS:");
            ConfigureDeviceName();
            ConfigureServerAddress();
            ConfigureUploadInterval();
            Console.WriteLine();
            Console.WriteLine("MONITORED ADAPTERS:");
            ConfigureMonitoredAdapters(_pcapDevices);
            Console.WriteLine("ALLOWED PROTOCOLS:");
            ConfigureMonitoredProtocols();
            Console.WriteLine();
            return _settings;
        }

        private void ConfigureDeviceName()
        {
            _settings.DeviceName = ConsoleTemplateProvider.GetUserStringInput("Enter name of this device");
        }

        private void ConfigureServerAddress()
        {
            _settings.ServerAddress = ConsoleTemplateProvider.GetUserStringInput("Enter server address(example \"http://localhost:31311\")");
        }

        private void ConfigureUploadInterval()
        {

            _settings.UploadInterval = ConsoleTemplateProvider.GetUserDoubleInput("Enter upload interval in seconds");
        }

        private void ConfigureMonitoredAdapters(List<PcapDevice> adapters)
        {
            _settings.MonitoredNetworkAdapters = new List<NetworkAdapterConfiguration>();

            foreach (var pcapDevice in adapters)
            {
                var response = ConsoleTemplateProvider.BuildYesNoDialog(
                    $"Enable monitoring on network adapter \"{pcapDevice.Interface.FriendlyName}\"",
                    "Enabled",
                    "Not enabled", true);

                if (response == UserDialogResponse.Positive)
                {
                    var customName = ConsoleTemplateProvider.GetUserStringInput(
                        "Enter custom name",
                        pcapDevice.Interface.FriendlyName);
                    _settings.MonitoredNetworkAdapters.Add(new NetworkAdapterConfiguration
                        {CustomName = customName, InterfaceFriendlyName = pcapDevice.Interface.FriendlyName});
                }

                Console.WriteLine();
            }
        }

        private void ConfigureMonitoredProtocols()
        {
            _settings.MonitoredProtocols = new Dictionary<string, bool>();

            foreach (var protocol in ProtocolsLists.AllProtocols)
            {
                var response = ConsoleTemplateProvider.BuildYesNoDialog($"Allow protocol {protocol}", "Allowed", "Not allowed", true);

                _settings.MonitoredProtocols.Add(protocol, response == UserDialogResponse.Positive);
            }
        }
    }
}