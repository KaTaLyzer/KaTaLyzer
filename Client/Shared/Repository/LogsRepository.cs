using System.Threading.Tasks;
using Shared.Repository.LogstashMessages;

namespace Shared.Repository
{
    public class LogsRepository : HttpServerRepository
    {
        public LogsRepository() : base(5)
        {

        }

        public async Task PersistLogsToServer(LogMessage logMessage)
        {
            await Persist(logMessage);
        }
    }
}
