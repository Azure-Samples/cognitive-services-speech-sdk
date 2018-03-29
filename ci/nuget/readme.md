# NuGet package creation

Files in this directory are used to generate a NuGet package.

The general call syntax to generate a package is:

```cmd
nuget.exe pack carbon.nuspec -Properties "Configuration=<[Release|Debug]>;Platform=<Win32|x64>;BuildNumber=<NUMBER>" 
```

For example:

```cmd
nuget pack carbon.nuspec -Properties "Configuration=Release;Platform=x64;BuildNumber=0.0.0.007"
```

Running this in the `nuget` directory will generate the NuGet package under the assumption that the created binaries are located in `..\..\build\x64`.
