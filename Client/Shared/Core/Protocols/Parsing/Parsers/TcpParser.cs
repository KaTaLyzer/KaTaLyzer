using System;
using System.Collections.Generic;
using PacketDotNet;
using Shared.Core.Capturing;
using Shared.Core.Protocols.Custom;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class TcpParser : ProtocolParserWithPayloadPacket<TcpPacket>
    {
        private readonly Dictionary<ushort, Func<TcpPacket, Packet>> _tcpPacketsMapping;

        public TcpParser()
        {
            _tcpPacketsMapping = new Dictionary<ushort, Func<TcpPacket, Packet>>
            {
                {80, (parentPacket) => new HttpPacket()},
                {443, (parentPacket) => new HttpsPacket()},
                {20, (parentPacket) => new FtpPacket()},
                {5060, (parentPacket) => new SipPacket()},
                {5061, (parentPacket) => new SipPacket()},
                {22, (parentPacket) => new SshPacket()},
            };
        }

        protected override Packet ParsePayloadPacket(TcpPacket packet)
        {
            Packet result;

            if (_tcpPacketsMapping.ContainsKey(packet.SourcePort))
            {
                result = _tcpPacketsMapping[packet.SourcePort]?.Invoke(packet);
            }
            else if (_tcpPacketsMapping.ContainsKey(packet.DestinationPort))
            {
                result = _tcpPacketsMapping[packet.DestinationPort]?.Invoke(packet);
            }
            else
            {
                result = null;
            }

            return result;
        }

        public override void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            var tcpPacket = (TcpPacket) packet;

            statisticsRecord.SourcePort = tcpPacket.SourcePort;
            statisticsRecord.DestinationPort = tcpPacket.DestinationPort;

            statisticsRecord.TransportLayer = tcpPacket.GetType().Name.Replace("Packet", "");

            TryParsePayloadIfEmpty(tcpPacket);
        }
    }
}