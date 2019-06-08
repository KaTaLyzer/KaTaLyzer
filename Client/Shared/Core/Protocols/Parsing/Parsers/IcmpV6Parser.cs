using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class IcmpV6Parser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var icmpV6Packet = (ICMPv6Packet) packet;
            statisticsRecord.InternetLayer = icmpV6Packet.GetType().Name.Replace("Packet", "");
        }
    }
}