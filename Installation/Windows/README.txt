PROBE Installation
1. Install .NET Framework 4.7.2 Runtime (if missing, popup is shown upon running KaTaLyzer)
2. Install Npcap https://nmap.org/npcap/
3. Run "KaTaLyzerWindows.exe"

Client Background mode
1. Make sure probe properly works in standard mode
2. Create Windows service in CMD with administrator rights
sc.exe create KaTaLyzerService binPath= "<path to KaTaLyzerWindows.exe > -b"
3. In Windows services and set other settings to service and start service

Backend Windows
1. Install Docker for Windows (https://docs.docker.com/docker-for-windows/install/)
2. Enable Hyper-V Virtualization
3. Run Docker
4. Open PowerShell
5. Navigate to "ElkDocker" folder
6. Run as root "docker-compose up --force-recreate --build". This recreates container

Optional:
Import default dashboard to Kibana. In Kibana navigate to Management->Saved Objects. Click Import and then import "KibanaSettings.json" located in Installation folder
DONE
