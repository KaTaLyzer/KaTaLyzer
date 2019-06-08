using System;
using System.Collections.Generic;
using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    internal class UdpParser : ProtocolParserWithPayloadPacket<UdpPacket>
    {
        private readonly Dictionary<ushort, Func<Packet, Packet>> _udpPacketsMapping;

        public UdpParser()
        {
            _udpPacketsMapping = new Dictionary<ushort, Func<Packet, Packet>>
            {
                {5060, (packet) => new SipPacket()},
                {5061, (packet) => new SipPacket()},
                {443, (packet) => new GoogleQuic()},
                {80, (packet) => new GoogleQuic()},
            };
        }

        protected override Packet ParsePayloadPacket(UdpPacket packet)
        {
            Packet payloadPacket = null;

            if (_udpPacketsMapping.ContainsKey(packet.SourcePort))
            {
                payloadPacket = _udpPacketsMapping[packet.SourcePort].Invoke(packet);
            }
            else if (_udpPacketsMapping.ContainsKey(packet.DestinationPort))
            {
                payloadPacket = _udpPacketsMapping[packet.DestinationPort].Invoke(packet);
            }

            return payloadPacket;
        }

        public override void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var udpPacket = (UdpPacket) packet;

            statisticsRecord.SourcePort = udpPacket.SourcePort;
            statisticsRecord.DestinationPort = udpPacket.DestinationPort;

            statisticsRecord.TransportLayer = udpPacket.GetType().Name.Replace("Packet", "");

            TryParsePayloadIfEmpty(udpPacket);
        }
    }
}