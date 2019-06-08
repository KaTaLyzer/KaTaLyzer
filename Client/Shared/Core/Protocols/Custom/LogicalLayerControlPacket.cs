using System.ComponentModel;
using System.Linq;
using System.Net.NetworkInformation;
using PacketDotNet;
using PacketDotNet.Utils;

namespace Shared.Core.Protocols.Custom
{
    sealed class LogicalLayerControlPacket : Packet
    {
        //LLC properties
        public byte DestinationServiceAccessPoint { get; private set; }
        public byte SourceServiceAccessPoint { get; private set; }
        public byte Control { get; private set; }

        //SNAP only properties
        [DefaultValue(null)]
        public int? OrganizationIdentification{ get; private set; }
        [DefaultValue(null)]
        public int? ProtocolId { get; private set; }

        public bool IsSnap { get; private set; }

        public PhysicalAddress DestinationHardwareAddress { get; }
        public PhysicalAddress SourceHardwareAddress { get; }

        public LogicalLayerControlPacket(EthernetPacket parentPacket)
        {
            ParentPacket = parentPacket;

            DestinationHardwareAddress = parentPacket.DestinationHwAddress;
            SourceHardwareAddress = parentPacket.SourceHwAddress;

            SetLogicalLayerControlHeader(parentPacket.PayloadData);
        }

        private void SetLogicalLayerControlHeader(byte[] packet)
        {
            DestinationServiceAccessPoint = packet[0];
            SourceServiceAccessPoint = packet[1];

            Control = packet[2];

            if (packet[0] == 170 && packet[1] == 170)
            {
                //LLC+SNAP
                IsSnap = true;
                Header = new ByteArraySegment(packet.Take(8).ToArray());

                OrganizationIdentification = Bytes.Skip(3).Take(3).ToArray().ConvertToInt32();
                ProtocolId = Bytes.Skip(6).Take(2).ToArray().ConvertToInt32();
            }
            else
            {
                //LLC
                IsSnap = false;
                Header = new ByteArraySegment(packet.Take(3).ToArray());
            }
        }
    }
}
