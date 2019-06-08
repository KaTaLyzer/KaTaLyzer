using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Shared.Logging;

namespace Shared.Configuration
{
    public class ConfigurationManager
    {
        public const string DefaultConfigurationFilePath = "config.json";
        private static ConfigurationManager _configurationManager;

        private Settings _settings;

        private string _pathToCurrentConfig;
        private bool _isConfigLoaded;

        private ConfigurationManager()
        {

        }

        public static ConfigurationManager GetInstance()
        {
            return _configurationManager ?? (_configurationManager = new ConfigurationManager());
        }

        public void LoadDefaultConfig()
        {
            LoadConfiguration(DefaultConfigurationFilePath);
        }

        public void LoadConfiguration(string filename)
        {
            if (File.Exists(filename))
            {
                using (var reader = new StreamReader(filename))
                {
                    string json = reader.ReadToEnd();
                    _settings = JsonConvert.DeserializeObject<Settings>(json);
                    _isConfigLoaded = true;
                    _pathToCurrentConfig = filename;
                    KaTaLyzerLogger.Log(LogLevel.Trace, "Configuration loaded").Wait();
                }
            }
            else
            {
                throw new FileNotFoundException($"File \"{filename}\" was not found.");
            }
        }

        public void SaveConfiguration(Settings settings, string filename, bool forceSave, Action<Settings, string> fileExistsAction)
        {
            if (File.Exists(filename) && !forceSave)
            {
                if (fileExistsAction != null)
                {
                    fileExistsAction.Invoke(settings, filename);
                }
                else
                {
                    throw new InvalidOperationException($"File \"{filename}\" already exists");
                }
            }
            else
            {
                WriteConfiguration(filename, settings);
            }

            _settings = settings;
            _isConfigLoaded = true;
            _pathToCurrentConfig = filename;
        }
        
        private void WriteConfiguration(string filename, Settings settings)
        {
            using (var file = File.CreateText(filename))
            using (var writer = new JsonTextWriter(file))
            {
                var serializer = new JsonSerializer {Formatting = Formatting.Indented};
                serializer.Serialize(writer, settings);
            }
        }

        public bool IsPacketTypeMonitored(Type packetType)
        {
            Type currentType = packetType;

            bool result = false;

            while (currentType != null)
            {
                string packetKey = currentType.Name.Replace("Packet", "");

                if (_settings.MonitoredProtocols.ContainsKey(packetKey))
                {
                    result = _settings.MonitoredProtocols[packetKey];
                    break;
                }

                currentType = currentType.BaseType;
            }

            return result;
        }

        public bool IsConfigLoaded()
        {
            return _isConfigLoaded;
        }

        public string GetPathToCurrentConfig()
        {
            return _pathToCurrentConfig;
        }

        public List<NetworkAdapterConfiguration> GetNetworkAdapterConfiguration()
        {
            return new List<NetworkAdapterConfiguration>(_settings.MonitoredNetworkAdapters);
        }

        public string GetDeviceName()
        {
            return _settings?.DeviceName;
        }

        public double GetUploadInterval()
        {
            return _settings.UploadInterval;
        }

        public string GetServerAddress()
        {
            return _settings?.ServerAddress;
        }

        public bool HasConfiguration()
        {
            return _settings != null;
        }
    }
}