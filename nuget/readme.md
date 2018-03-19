Files in this directory are used to generate a nuget package.

The general call syntax to generate a package:

    nuget.exe pack carbon.nuspec -Properties "Configuration=<[Release|Debug]>;Platform=<Win32|x64>;BuildNumber=<NUMBER>" 

For example:

    `nuget pack carbon.nuspec -Properties "Configuration=Release;Platform=x64;BuildNumber=0.0.0.007"`

    Running this in the `nuget` directory will generate the nuget package under the assumption that the created binaries are located in `build\x64`.

