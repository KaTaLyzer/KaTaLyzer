using System;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Shared.Configuration;
using Shared.Core;
using Shared.Repository.LogstashMessages;

namespace Shared.Repository
{
    public abstract class HttpServerRepository
    {
        private readonly TimeSpan _timeout;

        protected HttpServerRepository(int timeout)
        {
            _timeout = new TimeSpan(0, 0, timeout);
        }

        protected async Task Persist(LogstashMessage message)
        {
            using (var client = new HttpClient(){Timeout = _timeout })
            {
                var serializedData = message.ToJson();

                var content = new StringContent(serializedData, Encoding.UTF8, "application/json");

                var response = await client.PostAsync(ConfigurationManager.GetInstance().GetServerAddress(), content);

                response.EnsureSuccessStatusCode();
            }
        }
    }
}
