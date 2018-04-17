# Introduction 

This is the initial working project repository for the Microsoft Cognitive Services Speech SDK aka Carbon.

# Cloning the repo

* `git clone https://msasg.visualstudio.com/DefaultCollection/Skyman/_git/Carbon`
* `git submodule update --init`

> Note: currently recursive Git submodule (`--recursive`) initialization is not required.

# Building Carbon

* Create a `build` directory __inside__ the local `Carbon` repository: `cd Carbon && mkdir build && cd build`.

## Windows

* Run `cmake` to generate VS solution:
    * (32-bit platform): `cmake -G "Visual Studio 15" ..` 
    * (64-bit platform): `cmake -G "Visual Studio 15 Win64" ..` 
* Initialize repo or Unidec to ensure dependencies are in place:
    * `init.cmd` # Initalizes the repo, including updating Unidec for DEBUG builds
    * `init_unidec.cmd`, # Initializes Unidec dependencies (acoustic models, language models, etc.) 
    * `init_unidec.cmd DEBUG` # Initializes Unidec dependencies for use by DEBUG builds (acoustic models, language models, etc.) 
    * `init_unidec.cmd RELEASE` # Initializes Unidec dependencies for use by RELEASE builds (acoustic models, language models, etc.) 
* Open up `carbon.sln` and build using Visual Studio (alternatively run `cmake --build .`)

## Linux

* Install dependencies: 
    * `sudo apt-get update`
    * `sudo apt-get install build-essential zlib1g-dev libssl-dev libcurl4-openssl-dev swig python cmake python-dev alsa libasound2-dev pkg-config`
* Run `cmake` to generate makefiles:
    * (Release build): `cmake ..` 
    * (Debug build): `cmake -DCMAKE_BUILD_TYPE=Debug ..` 
* Run the build command: `cmake --build .`

