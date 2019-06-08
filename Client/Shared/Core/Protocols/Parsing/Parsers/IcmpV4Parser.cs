using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class IcmpV4Parser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var icmpV4Packet = (IcmpV4Packet) packet;
            statisticsRecord.InternetLayer = icmpV4Packet.GetType().Name.Replace("Packet", "");
        }
    }
}