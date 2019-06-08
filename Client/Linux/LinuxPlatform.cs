using System;
using System.Collections.Generic;
using System.Linq;
using Shared.Logging;
using Shared.RunningModes;
using SharpPcap.LibPcap;

namespace Linux
{
    public class LinuxPlatform : PlatformDependencies
    {
        public override List<PcapDevice> GetCurrentDevices()
        {
            try
            {
                LibPcapLiveDeviceList.Instance.Refresh();
                return new List<PcapDevice>(LibPcapLiveDeviceList.Instance.ToList())
                    .Where(device => device?.Interface?.FriendlyName != null).ToList();
            }
            catch (Exception e)
            {
                KaTaLyzerLogger.Log(LogLevel.Fatal, e).Wait();
                throw new Exception("Could not load network adapters. Check if you have installed libpcap library and you run KaTaLyzer as root.");
            }
        }

        public override void RunInBackground()
        {
            new BackgroundJob(this).Start().Wait();
        }

        public LinuxPlatform()
        {

        }
    }
}