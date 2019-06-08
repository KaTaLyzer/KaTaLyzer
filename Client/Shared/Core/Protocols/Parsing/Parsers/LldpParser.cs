using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class LldpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var lldpPacket = (LLDPPacket) packet;

            statisticsRecord.NetworkInterfaceLayer = lldpPacket.GetType().Name.Replace("Packet", "");
        }
    }
}