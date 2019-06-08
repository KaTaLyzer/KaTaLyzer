using System.Collections.Generic;
using System.Threading.Tasks;
using Shared.Configuration;
using Shared.Core;
using Shared.Logging;
using SharpPcap.LibPcap;

namespace Shared.RunningModes
{
    public class BackgroundJob
    {
        private readonly PlatformDependencies _dependencies;
        private KaTaLyzerWorker _kaTaLyzerWorker;

        public BackgroundJob(PlatformDependencies dependencies)
        {
            _dependencies = dependencies;
        }

        public async Task Start()
        {
            Stop();

            if (!ConfigurationManager.GetInstance().IsConfigLoaded())
            {
                await KaTaLyzerLogger.Log(LogLevel.Info, "No configuration file specified. Loading \"config.json\".");
                ConfigurationManager.GetInstance().LoadDefaultConfig();
            }

            _kaTaLyzerWorker = new KaTaLyzerWorker(_dependencies);
            _kaTaLyzerWorker.Start();
        }

        public void Stop()
        {
            _kaTaLyzerWorker?.Stop();
        }
    }
}
