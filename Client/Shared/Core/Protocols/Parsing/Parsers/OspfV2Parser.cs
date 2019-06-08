using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class OspfV2Parser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            statisticsRecord.TransportLayer = "OSPFv2";
        }
    }
}