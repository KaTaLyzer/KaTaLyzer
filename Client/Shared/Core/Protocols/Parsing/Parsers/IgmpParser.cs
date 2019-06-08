using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class IgmpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var igmpPacket = (IGMPv2Packet) packet;
            statisticsRecord.InternetLayer = igmpPacket.GetType().Name.Replace("Packet", "");
        }
    }
}