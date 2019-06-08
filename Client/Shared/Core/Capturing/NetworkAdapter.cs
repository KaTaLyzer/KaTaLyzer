using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Shared.Configuration;
using Shared.Logging;
using Shared.RunningModes;
using SharpPcap;
using SharpPcap.LibPcap;

namespace Shared.Core.Capturing
{
    public class NetworkAdapter
    {
        private PcapDevice _device;
        private readonly NetworkAdapterConfiguration _configuration;
        private readonly PlatformDependencies _dependencies;

        public string AdapterName => _configuration.CustomName;

        public EventHandler<RawCapture> PacketCaptured = null;
        public EventHandler<Exception> ErrorOccured = null;

        private bool _stopped;

        public NetworkAdapter(NetworkAdapterConfiguration configuration, PlatformDependencies dependencies)
        {
            _configuration = configuration;
            _dependencies = dependencies;
        }

        public void StopCapturing()
        {
            _stopped = true;
        }

        public async Task StartCapturing()
        {
            _stopped = false;

            await Task.Run(() =>
            {
                _device = _dependencies.GetActualDevice(_configuration.InterfaceFriendlyName);
                _device?.Open(DeviceMode.Promiscuous, 1000);

                while (!_stopped)
                {
                    try
                    {
                        RawCapture rawPacket = _device.GetNextPacket();

                        if (rawPacket != null) PacketCaptured?.Invoke(this, rawPacket);
                    }
                    catch (Exception e)
                    {
                        //try refresh device if something has changed in pc settings
                        var refreshedDevice = _dependencies.GetActualDevice(_configuration.InterfaceFriendlyName);
                        if (refreshedDevice != null)
                        {
                            _device = refreshedDevice;
                            _device.Open(DeviceMode.Promiscuous, 1000);
                        }
                        
                        ErrorOccured?.Invoke(this, e);
                        KaTaLyzerLogger.Log(LogLevel.Error, e, new KeyValuePair<string, object>("Adapter", this)).Wait();
                    }
                }

                _device?.Close();
            });
        }
    }
}