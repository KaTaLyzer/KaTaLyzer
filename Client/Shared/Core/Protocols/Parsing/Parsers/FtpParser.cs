using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class FtpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            FtpPacket ftpPacket = (FtpPacket) packet;
            statisticsRecord.ApplicationLayer = ftpPacket.GetType().Name.Replace("Packet", "");
        }
    }
}