using System;
using System.Collections.Generic;
using PacketDotNet;
using Shared.Core.Protocols.Custom;
using Shared.Core.Protocols.Parsing;
using Shared.Core.Protocols.Parsing.Parsers;

namespace Shared.Core.Protocols
{
    public static class ProtocolsLists
    {
        public static readonly List<string> AllProtocols = new List<string>
        {
            typeof(ARPPacket).Name.Replace("Packet", ""),
            typeof(EthernetPacket).Name.Replace("Packet", ""),
            typeof(ICMPv4Packet).Name.Replace("Packet", ""),
            typeof(ICMPv6Packet).Name.Replace("Packet", ""),
            typeof(IGMPv2Packet).Name.Replace("Packet", ""),
            typeof(IPv4Packet).Name.Replace("Packet", ""),
            typeof(IPv6Packet).Name.Replace("Packet", ""),
            typeof(LinuxSLLPacket).Name.Replace("Packet", ""),
            typeof(LLDPPacket).Name.Replace("Packet", ""),
            typeof(TcpPacket).Name.Replace("Packet", ""),
            typeof(UdpPacket).Name.Replace("Packet", ""),
            typeof(FtpPacket).Name.Replace("Packet", ""),
            typeof(HttpPacket).Name.Replace("Packet", ""),
            typeof(HttpsPacket).Name.Replace("Packet", ""),
            typeof(SipPacket).Name.Replace("Packet", ""),
            typeof(LogicalLayerControlPacket).Name.Replace("Packet", ""),
            typeof(CdpPacket).Name.Replace("Packet", ""),
            typeof(WakeOnLanPacket).Name.Replace("Packet", ""),
            typeof(Ieee8021QPacket).Name.Replace("Packet", ""),
            typeof(GREPacket).Name.Replace("Packet", ""),
            typeof(OSPFv2Packet).Name.Replace("Packet", ""),
            typeof(GoogleQuic).Name.Replace("Packet", ""),
            typeof(SshPacket).Name.Replace("Packet", ""),
        };

        public static readonly Dictionary<Type, IProtocolParser> ProtocolParsersMapper = new Dictionary
            <Type, IProtocolParser>
            {
                {typeof(ARPPacket), new ArpParser()},
                {typeof(EthernetPacket), new EthernetParser()},
                {typeof(ICMPv4Packet), new IcmpV4Parser()},
                {typeof(ICMPv6Packet), new IcmpV6Parser()},
                {typeof(IGMPv2Packet), new IgmpParser()},
                {typeof(IPv4Packet), new IpParser()},
                {typeof(IPv6Packet), new IpParser()},
                {typeof(LinuxSLLPacket), new LinuxSllParser()},
                {typeof(TcpPacket), new TcpParser()},
                {typeof(UdpPacket), new UdpParser()},
                {typeof(LLDPPacket), new LldpParser()},
                {typeof(FtpPacket), new FtpParser()},
                {typeof(HttpPacket), new HttpParser()},
                {typeof(HttpsPacket), new HttpsParser()},
                {typeof(SipPacket), new SipParser()},
                {typeof(LogicalLayerControlPacket), new LogicalLinkControlParser()},
                {typeof(CdpPacket), new CdpParser()},
                {typeof(WakeOnLanPacket), new WakeOnLanParser()},
                {typeof(Ieee8021QPacket), new Ieee8021QParser()},
                {typeof(OSPFv2Packet), new OspfV2Parser()},
                {typeof(OSPFv2HelloPacket), new OspfV2Parser()},
                {typeof(OSPFv2LSRequestPacket), new OspfV2Parser()},
                {typeof(OSPFv2DDPacket), new OspfV2Parser()},
                {typeof(OSPFv2LSUpdatePacket), new OspfV2Parser()},
                {typeof(OSPFv2LSAPacket), new OspfV2Parser()},
                {typeof(GoogleQuic), new GoogleQuicParser()},
                {typeof(SshPacket), new SshParser()},
            };
    }
}