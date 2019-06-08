using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Shared.Core;
using Shared.Repository;
using Shared.Repository.LogstashMessages;

namespace Shared.Logging
{
    public static class KaTaLyzerLogger
    {
        private static readonly string LogPath = "logs.txt";
        private static readonly ReaderWriterLock Locker = new ReaderWriterLock();

        public static async Task Log(LogLevel logLevel, Exception exception,
            params KeyValuePair<string, object>[] additionalInfo)
        {
            var body = new Dictionary<string, string>
            {
                {"Type", exception.GetType().ToString()},
                {"Message", exception.Message},
                {"StackTrace", exception.StackTrace}
            };

            await LogToServerAndFile(logLevel, body, additionalInfo);
        }

        public static async Task Log(LogLevel logLevel, string message, params KeyValuePair<string, object>[] additionalInfo)
        {
            var body = new Dictionary<string, string>
            {
                {"Message", message},
            };

            await LogToServerAndFile(logLevel, body, additionalInfo);
        }

        private static async Task LogToServerAndFile(LogLevel logLevel, object message, params KeyValuePair<string, object>[] additionalInfo)
        {
            LogMessage logMessage = new LogMessage
            {
                LogLevel = logLevel,
                Body = message,
                AdditionalInformation = additionalInfo,
                CreatedAt = DateTime.UtcNow
            };

            try
            {
                //log to server
                await new LogsRepository().PersistLogsToServer(logMessage);
            }
            catch
            {
                // ignored
            }

            //log to file
            LogToFile(logMessage);
        }

        private static void LogToFile(LogMessage logMessage)
        {
            try
            {
                Locker.AcquireWriterLock(int.MaxValue);
                File.AppendAllLines(LogPath, new List<string>() { logMessage.ToJson() });
            }
            finally
            {
                Locker.ReleaseWriterLock();
            }
        }
    }
}