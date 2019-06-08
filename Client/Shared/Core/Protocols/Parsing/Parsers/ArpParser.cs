using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class ArpParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var arpPacket = (ArpPacket) packet;

            if (arpPacket.Operation == ArpOperation.ReplyReverse || arpPacket.Operation == ArpOperation.RequestReverse)
            {
                statisticsRecord.InternetLayer = "RARP";
            }
            else
            {
                statisticsRecord.InternetLayer = arpPacket.GetType().Name.Replace("Packet", "");
            }
        }
    }
}