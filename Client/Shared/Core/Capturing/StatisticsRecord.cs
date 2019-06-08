using System;
using System.ComponentModel;
using Newtonsoft.Json;

namespace Shared.Core.Capturing
{
    public class StatisticsRecord
    {
        public StatisticsRecord()
        {

        }

        public StatisticsRecord(StatisticsRecord otherRecord)
        {
            NetworkInterfaceLayer = otherRecord.NetworkInterfaceLayer;
            InternetLayer = otherRecord.InternetLayer;
            TransportLayer = otherRecord.TransportLayer;
            ApplicationLayer = otherRecord.ApplicationLayer;
            SourceMacAddress = otherRecord.SourceMacAddress;
            DestinationMacAddress = otherRecord.DestinationMacAddress;
            SourceIpAddress = otherRecord.SourceIpAddress;
            DestinationIpAddress = otherRecord.DestinationIpAddress;
            SourcePort = otherRecord.SourcePort;
            DestinationPort = otherRecord.DestinationPort;
            Size = otherRecord.Size;
            CapturedAt = otherRecord.CapturedAt;
        }

        [JsonProperty(Order = 1)]
        [DefaultValue(null)]
        public string NetworkInterfaceLayer { get; set; }

        [JsonProperty(Order = 2)]
        [DefaultValue(null)]
        public string InternetLayer { get; set; }

        [JsonProperty(Order = 3)]
        [DefaultValue(null)]
        public string TransportLayer { get; set; }

        [JsonProperty(Order = 4)]
        [DefaultValue(null)]
        public string ApplicationLayer { get; set; }

        [JsonProperty(Order = 5)]
        [DefaultValue(null)]
        public string SourceMacAddress { get; set; }

        [JsonProperty(Order = 6)]
        [DefaultValue(null)]
        public string DestinationMacAddress { get; set; }

        [JsonProperty(Order = 7)]
        [DefaultValue(null)]
        public string SourceIpAddress { get; set; }

        [JsonProperty(Order = 8)]
        [DefaultValue(null)]
        public string DestinationIpAddress { get; set; }

        [JsonProperty(Order = 9)]
        [DefaultValue(null)]
        public int? SourcePort { get; set; }

        [JsonProperty(Order = 10)]
        [DefaultValue(null)]
        public int? DestinationPort { get; set; }

        [JsonProperty(Order = 11)]
        [DefaultValue(null)]
        public int Size { get; set; }

        [JsonProperty(Order = 12)]
        [DefaultValue(null)]
        public DateTime CapturedAt { get; set; }
    }
}