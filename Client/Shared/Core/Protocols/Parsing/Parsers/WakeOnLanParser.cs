using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class WakeOnLanParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            WakeOnLanPacket wakeOnLanPacket = (WakeOnLanPacket) packet;
            statisticsRecord.NetworkInterfaceLayer = wakeOnLanPacket.GetType().Name.Replace("Packet", "");
        }
    }
}