using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class HttpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var httpPacket = (HttpPacket) packet;
            statisticsRecord.ApplicationLayer = httpPacket.GetType().Name.Replace("Packet", "");
        }
    }
}