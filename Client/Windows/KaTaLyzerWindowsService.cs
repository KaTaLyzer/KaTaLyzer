using System.ServiceProcess;
using System.Threading;
using Shared.RunningModes;

namespace Windows
{
    public partial class KaTaLyzerWindowsService : ServiceBase
    {
        private readonly WindowsPlatform _dependencies;
        private BackgroundJob _backgroundJob;

        public KaTaLyzerWindowsService(WindowsPlatform dependencies)
        {
            _dependencies = dependencies;
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            base.OnStart(args);

            Thread workerThread = new Thread(() =>
            {
                _backgroundJob = new BackgroundJob(_dependencies);
                _backgroundJob.Start().Wait();
            });

            workerThread.Start();
        }

        protected override void OnStop()
        {
            base.OnStop();
            _backgroundJob?.Stop();
        }
    }
}
