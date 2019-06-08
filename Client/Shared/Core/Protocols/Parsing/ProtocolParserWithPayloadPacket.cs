using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing
{
    public abstract class ProtocolParserWithPayloadPacket<TP> : IProtocolParser where TP : Packet
    {
        protected void TryParsePayloadIfEmpty(TP packet)
        {
            if (packet.PayloadPacket == null)
            {
                Packet parsedPacket = ParsePayloadPacket(packet);

                if (parsedPacket != null)
                {
                    packet.PayloadPacket = parsedPacket;
                }
            }
        }

        protected abstract Packet ParsePayloadPacket(TP packet);

        public abstract void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord);
    }
}