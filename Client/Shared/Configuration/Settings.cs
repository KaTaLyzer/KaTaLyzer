using System.Collections.Generic;

namespace Shared.Configuration
{
    public class Settings
    {
        public double UploadInterval { get; set; }
        public string DeviceName { get; set; }
        public List<NetworkAdapterConfiguration> MonitoredNetworkAdapters { get; set; }
        public Dictionary<string, bool> MonitoredProtocols { get; set; }
        public string ServerAddress { get; set; }
    }
}