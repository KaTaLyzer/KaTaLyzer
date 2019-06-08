using PacketDotNet;
using Shared.Core.Capturing;

namespace Shared.Core.Protocols.Parsing
{
    public interface IProtocolParser
    {
        void ParseProtocol(Packet packet, StatisticsRecord statisticsRecord);
    }
}