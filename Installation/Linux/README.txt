PROBE Installation
1. Install libpcap (sudo apt-get install libpcap-dev)
2. Install .NET Core 2.1 Runtime (https://dotnet.microsoft.com/download/linux-package-manager/ubuntu18-04/runtime-2.1.2)
3. Run "sudo dotnet KaTaLyzerLinux.dll"

Installation ELK STACK
1. Install Docker for Linux (https://docs.docker.com/install/linux/docker-ce/ubuntu/)
2. Install docker-compose for Linux (https://docs.docker.com/compose/install/)
3. Open Terminal
4. Navigate to "ElkDocker" folder
5. Run as root "docker-compose up --force-recreate --build". This recreates container

If Elasticsearch won't start try edit value or add line "vm.max_map_count=262144" in file "/etc/sysctl.conf" and repeat from step 5.
https://www.elastic.co/guide/en/elasticsearch/reference/current/vm-max-map-count.html

If it won't read logs and don't forget to check Elasticsearch minimum system requirements.

Optional:
Import default dashboard to Kibana. In Kibana navigate to Management->Saved Objects. Click Import and then import "KibanaSettings.json" located in Installation folder
DONE
