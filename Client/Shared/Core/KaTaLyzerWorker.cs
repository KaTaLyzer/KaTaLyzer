using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Timers;
using Shared.Configuration;
using Shared.Core.Capturing;
using Shared.Logging;
using Shared.Repository;
using Shared.RunningModes;
using SharpPcap;

namespace Shared.Core
{
    public class KaTaLyzerWorker
    {
        private readonly PlatformDependencies _platformDependencies;
        private readonly object _startStopSequence = new object();
        private readonly object _tasksLock = new object();

        private readonly List<NetworkAdapterStatistics> _networkAdaptersStatistics;
        private readonly object _statisticLock = new object();

        private readonly Timer _timer;

        private DateTime _capturingStartedAt;

        public event EventHandler Started; 
        public event EventHandler Stopped;
        public event EventHandler<Exception> Error;
        public event Action<StatisticsPersistedEventArgs> StatisticsPersisted;

        private readonly List<Task> _runningTasks;

        private readonly List<NetworkAdapter> _adapters;

        public KaTaLyzerWorker(PlatformDependencies platformDependencies)
        {
            if (!ConfigurationManager.GetInstance().HasConfiguration())
            {
                throw new InvalidOperationException("Can't start KaTaLyzer without configuration");
            }

            _platformDependencies = platformDependencies;

            _networkAdaptersStatistics = new List<NetworkAdapterStatistics>();
            _runningTasks = new List<Task>();


            _adapters = ConfigurationManager.GetInstance().GetNetworkAdapterConfiguration().Select(configuration =>
            {
                NetworkAdapter adapter = null;


                adapter = new NetworkAdapter(configuration, platformDependencies);
                adapter.PacketCaptured += ProcessPacket;
                adapter.ErrorOccured += AdapterErrorOccured;

                return adapter;
            }).ToList();

            ulong uploadedCount = 0;

            _timer = new Timer { Interval = ConfigurationManager.GetInstance().GetUploadInterval() * 1000 };
            _timer.Elapsed += async (sender, eventArgs) =>
            {
                var statisticsRepository = new StatisticsRepository();

                try
                {
                    await statisticsRepository.PersistStatistics(TakeNetworkAdaptersStatistics());
                    uploadedCount++;

                    StatisticsPersisted?.Invoke(new StatisticsPersistedEventArgs(_capturingStartedAt, DateTime.Now, uploadedCount));
                }
                catch (Exception e)
                {
                    await KaTaLyzerLogger.Log(LogLevel.Error, e);
                    Error?.Invoke(this, e);
                }
            };
        }

        public void Start()
        {
            Stop();

            lock (_startStopSequence)
            {
                _adapters.ForEach(adapter =>
                {
                    _runningTasks.Add(adapter.StartCapturing());
                });

                _timer.Start();

                Started?.Invoke(this, EventArgs.Empty);

                _capturingStartedAt = DateTime.Now;

                KaTaLyzerLogger.Log(LogLevel.Trace, "Worker started").Wait();
            }

            lock (_tasksLock)
            {
                _runningTasks.ForEach(x => { x.Wait(); });
            }

            _timer.Stop();

            Stopped?.Invoke(this, EventArgs.Empty);

            KaTaLyzerLogger.Log(LogLevel.Trace, "Worker stopped").Wait();
        }

        public void Stop()
        {
            _timer.Stop();

            lock (_startStopSequence)
            {
                _adapters.ForEach(x => x.StopCapturing());

                lock (_tasksLock)
                {
                    _runningTasks.ForEach(x => { x.Wait(); });

                    _runningTasks.Clear();
                }
            }
        }

        public List<NetworkAdapterStatistics> TakeNetworkAdaptersStatistics()
        {
            List<NetworkAdapterStatistics> oldStatistics;

            lock (_statisticLock)
            {
                oldStatistics = new List<NetworkAdapterStatistics>(_networkAdaptersStatistics);

                _networkAdaptersStatistics.Clear();
            }
            var x = oldStatistics.SummarizeNetworkAdapterStatistics();
            return x;
        }

        protected async void ProcessPacket(object networkAdapter, RawCapture rawCapture)
        {
            try
            {
                StatisticsRecord capturedPacket = rawCapture.ConvertToStatistics();

                if (capturedPacket != null)
                {
                    IncreaseStatistics(networkAdapter, capturedPacket);
                }
            }
            catch (Exception e)
            {
                await KaTaLyzerLogger.Log(LogLevel.Error, e, new KeyValuePair<string, object>("CapturedData", rawCapture.Data));
            }
        }

        private void AdapterErrorOccured(object sender, Exception e)
        {
            Error?.Invoke(this, e);
        }

        private void IncreaseStatistics(object adapter, StatisticsRecord capturedPacket)
        {
            NetworkAdapter networkAdapter = (NetworkAdapter)adapter;

            lock (_statisticLock)
            {
                NetworkAdapterStatistics adapterStatistics = _networkAdaptersStatistics.FirstOrDefault(x => x.NetworkAdapter == networkAdapter);

                if (adapterStatistics == null)
                {
                    _networkAdaptersStatistics.Add(new NetworkAdapterStatistics()
                    {
                        NetworkAdapter = networkAdapter,
                        Statistics = new List<StatisticsRecord>()
                        {
                            capturedPacket

                        }
                    });
                }
                else
                {
                    adapterStatistics.Statistics.Add(capturedPacket);
                }
            }
        }
    }
}