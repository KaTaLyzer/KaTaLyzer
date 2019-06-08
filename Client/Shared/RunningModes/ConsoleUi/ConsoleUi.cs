using System;
using System.Threading.Tasks;
using Shared.Core;
using Shared.RunningModes.ConsoleUi.Dialogs;

namespace Shared.RunningModes.ConsoleUi
{
    public class ConsoleUi
    {
        private readonly PlatformDependencies _dependencies;
        private readonly MainMenuDialogs _dialogs;
        private KaTaLyzerWorker _kaTaLyzerWorker;

        public ConsoleUi(PlatformDependencies dependencies)
        {
            _dependencies = dependencies;

            _dialogs = new MainMenuDialogs();
        }

        public void ShowMainMenu()
        {
            _dialogs.ShowMainMenuOptions(_dependencies.GetCurrentDevices(), StartWorker);
        }

        public void InvalidArgumentsWerePassed()
        {
            _dialogs.InvalidArgumentsWerePassed();
        }

        public void ShowHelp()
        {
            _dialogs.ShowHelp();
        }

        private void StartWorker()
        {
            CreateWorkerIfNeeded();

            Task.Run(() => { _kaTaLyzerWorker.Start(); });

            _dialogs.WaitUntilKeyIsPressed(ConsoleKey.Escape);

            _kaTaLyzerWorker.Stop();
        }

        private void CreateWorkerIfNeeded()
        {
            if (_kaTaLyzerWorker == null)
            {
                _kaTaLyzerWorker = new KaTaLyzerWorker(_dependencies);

                _kaTaLyzerWorker.Started += (sender, args) =>
                {
                    _dialogs.CapturingStarted();
                };
                _kaTaLyzerWorker.StatisticsPersisted += args =>
                {
                    _dialogs.RefreshRunningWorkerInfo(args.WorkerStarted, args.LastPersistingTime, args.Count);
                };
                _kaTaLyzerWorker.Stopped += (sender, args) =>
                {
                    _dialogs.CapturingStopped();
                };
                _kaTaLyzerWorker.Error += (sender, args) =>
                {
                    _dialogs.ErrorOccuredDuringCapturing();
                };
            }
        }
    }
}