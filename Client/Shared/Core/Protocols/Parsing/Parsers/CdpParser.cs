using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class CdpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            CdpPacket cdpPacket = (CdpPacket) packet;
            statisticsRecord.NetworkInterfaceLayer = cdpPacket.GetType().Name.Replace("Packet", "");
        }
    }
}
