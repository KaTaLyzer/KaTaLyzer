using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceProcess;
using Shared.Logging;
using Shared.RunningModes;
using SharpPcap.LibPcap;
using SharpPcap.WinPcap;

namespace Windows
{
    public class WindowsPlatform : PlatformDependencies
    {
        public override List<PcapDevice> GetCurrentDevices()
        {
            try
            {
                WinPcapDeviceList.Instance.Refresh();
                return new List<PcapDevice>(WinPcapDeviceList.Instance.ToList())
                    .Where(device => device?.Interface?.FriendlyName != null).ToList();
            }
            catch (Exception e)
            {
                KaTaLyzerLogger.Log(LogLevel.Fatal, e).Wait();
                throw new Exception("Could not load network adapters. Check if you have installed npcap library and you run KaTaLyzer as administrator.");
            }
        }

        public override void RunInBackground()
        {
            var servicesToRun = new ServiceBase[]
            {
                new KaTaLyzerWindowsService(this)
            };
            ServiceBase.Run(servicesToRun);
        }

        public WindowsPlatform() 
        {

        }
    }
}