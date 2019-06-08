using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class HttpsParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var httpsPacket = (HttpsPacket) packet;
            statisticsRecord.ApplicationLayer = httpsPacket.GetType().Name.Replace("Packet", "");
        }
    }
}