using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class SipParser :IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            SipPacket sipPacket = (SipPacket) packet;
            statisticsRecord.ApplicationLayer = sipPacket.GetType().Name.Replace("Packet", "");
        }
    }
}
