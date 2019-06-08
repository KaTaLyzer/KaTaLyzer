using System.Net.NetworkInformation;
using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class LinuxSllParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var linuxSllPacket = (LinuxSllPacket) packet;
            statisticsRecord.NetworkInterfaceLayer = linuxSllPacket.GetType().Name.Replace("Packet", "");

            statisticsRecord.SourceMacAddress = new PhysicalAddress(linuxSllPacket.LinkLayerAddress).FormatMacAddress();
        }
    }
}