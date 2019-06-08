BASEDIR=$(dirname "$0")
cd "$BASEDIR"
nohup dotnet "KaTaLyzerLinux.dll" "-b $@" > /dev/null 2>&1 &
