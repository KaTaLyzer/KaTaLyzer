using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    class LogicalLinkControlParser : ProtocolParserWithPayloadPacket<LogicalLayerControlPacket>
    {
        protected override Packet ParsePayloadPacket(LogicalLayerControlPacket packet)
        {
            Packet payloadPacket = null;

            if (packet.IsSnap)
            {
                if (packet.DestinationHardwareAddress.ToString().Equals(""))
                {
                    if (packet.OrganizationIdentification == 12)
                    {
                        if (packet.ProtocolId == 8192)
                        {
                            payloadPacket = new CdpPacket();
                        }
                    }
                }
            }

            return payloadPacket;
        }

        public override void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            LogicalLayerControlPacket llcPacket = (LogicalLayerControlPacket) packet;

            statisticsRecord.NetworkInterfaceLayer = llcPacket.IsSnap ? "Ieee802.3LLC+SNAP" : "Ieee802.3LLC";

            TryParsePayloadIfEmpty(llcPacket);
        }
    }
}