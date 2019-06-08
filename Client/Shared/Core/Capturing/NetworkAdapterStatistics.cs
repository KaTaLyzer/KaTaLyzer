using System.Collections.Generic;
using Newtonsoft.Json;

namespace Shared.Core.Capturing
{
    public class NetworkAdapterStatistics
    {
        [JsonIgnore]
        public NetworkAdapter NetworkAdapter { get; set; }

        public string AdapterName => NetworkAdapter.AdapterName;

        public List<StatisticsRecord> Statistics { get; set; }
    }
}