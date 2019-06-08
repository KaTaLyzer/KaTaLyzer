using System.Collections.Generic;
using Shared.Core.Capturing;

namespace Shared.Repository.LogstashMessages
{
    public class StatisticsMessage : LogstashMessage
    {
        public List<NetworkAdapterStatistics> AdaptersStatistics { get; set; }

        public override string MessageContent => "statistics";
    }
}