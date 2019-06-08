using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class EthernetParser : ProtocolParserWithPayloadPacket<EthernetPacket>
    {
        protected override Packet ParsePayloadPacket(EthernetPacket packet)
        {
            return (int) packet.Type <= 1500 ? new LogicalLayerControlPacket(packet) : null;
        }

        public override void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            EthernetPacket ethernetPacket = (EthernetPacket) packet;
            statisticsRecord.SourceMacAddress = ethernetPacket.SourceHardwareAddress.FormatMacAddress();
            statisticsRecord.DestinationMacAddress = ethernetPacket.DestinationHardwareAddress.FormatMacAddress();

            statisticsRecord.NetworkInterfaceLayer = ethernetPacket.GetType().Name.Replace("Packet", "");

            TryParsePayloadIfEmpty(ethernetPacket);
        }
    }
}