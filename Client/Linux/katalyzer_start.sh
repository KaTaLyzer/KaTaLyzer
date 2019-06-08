BASEDIR=$(dirname "$0")
cd "$BASEDIR"
dotnet "KaTaLyzerLinux.dll" "$@"
