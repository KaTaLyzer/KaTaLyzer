using System;

namespace Shared.Core
{
    public class StatisticsPersistedEventArgs : EventArgs
    {
        public StatisticsPersistedEventArgs(DateTime workerStarted, DateTime lastPersistingTime, ulong count)
        {
            WorkerStarted = workerStarted;
            LastPersistingTime = lastPersistingTime;
            Count = count;
        }

        public DateTime WorkerStarted { get; }
        public DateTime LastPersistingTime { get; }
        public ulong Count { get; }
    }
}
