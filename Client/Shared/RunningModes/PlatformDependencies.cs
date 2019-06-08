using System.Collections.Generic;
using System.Linq;
using SharpPcap.LibPcap;

namespace Shared.RunningModes
{
    public abstract class PlatformDependencies
    {
        public abstract List<PcapDevice> GetCurrentDevices();

        protected PlatformDependencies()
        {

        }
        
        public abstract void RunInBackground();

        public PcapDevice GetActualDevice(string interfaceFriendlyName)
        {
            List<PcapDevice> allDevices = GetCurrentDevices();

            PcapDevice result  = allDevices.FirstOrDefault(newDevice => newDevice.Interface.FriendlyName == interfaceFriendlyName);

            return result;
        }
    }
}
