# NuGet package creation

Files in this directory are used to generate a NuGet package.

There is a single package consuming all (Windows) build flavors.
Hence, prior to creating this package, all builds need to be created and drops need to be rolled:

```cmd
cd <repository-root>
mkdir build\drop
mkdir build\Win32
cd build\win32
msbuild /m /p:Platform=Win32 /p:Configuration=Release carbon.sln
c:\Program Files\Git\bin\bash.exe" Win32 Release ..\drop
cd ..\..
mkdir build\x64
cmake -G "Visual Studio 15 2017 Win64" ..\..
msbuild /m /p:Platform=x64 /p:Configuration=Debug carbon.sln
c:\Program Files\Git\bin\bash.exe" x64 Debug ..\drop
msbuild /m /p:Platform=x64 /p:Configuration=Release carbon.sln
c:\Program Files\Git\bin\bash.exe" x64 Release ..\drop
cd ..\..
```

Afterwards, the package can be created, pointing to the drop directory and specifying a build number:

```cmd
nuget.exe pack carbon.nuspec -Properties "WindowsDropDir=..\..\build\drop;WindowsUwpDropDir=..\..\build\drop\uwp;LinuxDropDir=..\..\build\drop;OSXDropDir=..\..\build\drop\macOS;AndroidDropDir=..\..\build\drop;iOSDropDir=..\..\build\drop\iOS;RedistDir=..\..\;Version=1.1.1"
Attempting to build package from 'carbon.nuspec'.
```

For example:

```cmd
nuget.exe pack carbon.nuspec -Properties "DropDir=..\..\build\drop;Version=0.1.2"
```
