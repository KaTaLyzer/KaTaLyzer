using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text.RegularExpressions;
using Newtonsoft.Json;
using PacketDotNet;
using Shared.Configuration;
using Shared.Core.Capturing;
using Shared.Core.Protocols;
using SharpPcap;

namespace Shared.Core
{   
    public static class ExtensionMethods
    {
        public static List<NetworkAdapterStatistics> SummarizeNetworkAdapterStatistics(
            this List<NetworkAdapterStatistics> networkAdapterStatistics)
        {
            return networkAdapterStatistics.Select(adapterStatistics =>
            {
                return new NetworkAdapterStatistics()
                {
                    NetworkAdapter = adapterStatistics.NetworkAdapter, Statistics = adapterStatistics.Statistics
                        .GroupBy(x =>
                            new
                            {
                                LinkLayer = x.NetworkInterfaceLayer,
                                NetworkLayer = x.InternetLayer,
                                x.TransportLayer,
                                x.ApplicationLayer,
                                x.SourceMacAddress,
                                x.DestinationMacAddress,
                                x.SourceIpAddress,
                                x.DestinationIpAddress,
                                x.SourcePort,
                                x.DestinationPort,
                            })
                        .Select(g =>
                        {
                            return new StatisticsRecord(g.First()) {Size = g.Sum(x => x.Size)};
                        }).ToList()
                };
            }).ToList();
        }

        public static StatisticsRecord ConvertToStatistics(this RawCapture rawCapture)
        {
            var packet = Packet.ParsePacket(rawCapture.LinkLayerType, rawCapture.Data);

            if (rawCapture.LinkLayerType == LinkLayers.Null)
            {
                packet = packet.PayloadPacket;
            }

            var statisticsRecord = new StatisticsRecord
            {
                Size = packet.TotalPacketLength,
                CapturedAt = rawCapture.Timeval.Date
            };

            var discardStatistics = true;

            while (packet != null && ConfigurationManager.GetInstance().IsPacketTypeMonitored(packet.GetType()))
            {
                discardStatistics = false;

                ProtocolsLists.ProtocolParsersMapper[packet.GetType()].ParseProtocol(packet, statisticsRecord);

                packet = packet.PayloadPacket;
            }

            if (discardStatistics) statisticsRecord = null;

            return statisticsRecord;
        }

        public static int ConvertToInt32(this byte[] bytesToConvert)
        {
            List<byte> workingBytes = new List<byte>(bytesToConvert);

            while (workingBytes.Count < 4)
            {
                workingBytes.Insert(0, 0);
            }

            if (BitConverter.IsLittleEndian)
            {
                workingBytes.Reverse();
            }

            return BitConverter.ToInt32(workingBytes.ToArray(), 0);
        }

        public static string FormatMacAddress(this PhysicalAddress macAddress)
        {
            var regex = string.Concat(Enumerable.Repeat("([a-fA-F0-9]{2})", 6));

            return macAddress != null ? Regex.Replace(macAddress.ToString(), regex, "$1:$2:$3:$4:$5:$6") : null;
        }

        public static string ToJson(this object value)
        {
            return JsonConvert.SerializeObject(value,
                new JsonSerializerSettings
                {
                    DateFormatString = "yyyy-MM-dd'T'HH:mm:ss.fffZ",
                    NullValueHandling = NullValueHandling.Ignore,
                    Formatting = Formatting.Indented
                });
        }
    }
}