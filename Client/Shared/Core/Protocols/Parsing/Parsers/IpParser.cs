using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class IpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var ipPacket = (IPPacket) packet;

            statisticsRecord.DestinationIpAddress = ipPacket.DestinationAddress.ToString();
            statisticsRecord.SourceIpAddress = ipPacket.SourceAddress.ToString();
            statisticsRecord.InternetLayer = ipPacket.GetType().Name.Replace("Packet", "");
        }
    }
}