using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Shared.Core.Capturing;
using Shared.Repository.LogstashMessages;

namespace Shared.Repository
{
    public class StatisticsRepository : HttpServerRepository
    {

        public StatisticsRepository() : base(15)
        {

        }

        public async Task PersistStatistics(List<NetworkAdapterStatistics> statistics)
        {
            if (statistics.Any())
            {
                await Persist(new StatisticsMessage { AdaptersStatistics = statistics });
            }
        }
    }
}