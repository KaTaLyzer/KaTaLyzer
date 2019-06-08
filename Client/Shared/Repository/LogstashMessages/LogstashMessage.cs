using Shared.Configuration;

namespace Shared.Repository.LogstashMessages
{
    public abstract class LogstashMessage
    {
        public string DeviceName => ConfigurationManager.GetInstance().GetDeviceName();

        public abstract string MessageContent { get; }
    }
}
