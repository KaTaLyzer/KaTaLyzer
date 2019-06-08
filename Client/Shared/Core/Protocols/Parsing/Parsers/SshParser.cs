using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class SshParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            statisticsRecord.ApplicationLayer = "SSH";
        }
    }
}
