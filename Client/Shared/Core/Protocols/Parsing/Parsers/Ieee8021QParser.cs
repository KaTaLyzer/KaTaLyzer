using System;
using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing.Parsers
{
    public class Ieee8021QParser : IProtocolParser
    {
        public void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord)
        {
            Ieee8021QPacket vlanPacket = (Ieee8021QPacket) packet;

            statisticsRecord.NetworkInterfaceLayer = "Ieee802.1Q";
        }
    }
}