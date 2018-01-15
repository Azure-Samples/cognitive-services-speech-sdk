# Introduction 

This is the initial working project repository for the Client Speech SDK aka Carbon

# Cloning the repo

* `git clone https://msasg.visualstudio.com/DefaultCollection/Skyman/_git/Carbon`
* `git submodule init`
* `git submodule update --recursive`

# Building Carbon

* Create a `build` directory __inside__ the local `Carbon` repository: `cd Carbon && mkdir build && cd build`.

## Windows

* Run `cmake` to generate VS solution:
    * (32-bit platform): `cmake -G "Visual Studio 15" ..` 
    * (64-bit platform): `cmake -G "Visual Studio 15 Win64" ..` 
* Open up `carbon.sln` and build using Visual Studio (alternatively run `cmake --build .`)

## Linux

* Install dependencies: 
    * `sudo apt-get update`
    * `sudo apt-get build-essential zlib1g-dev libssl-dev libcurl4-openssl-dev`
* Run `cmake` to generate makefiles:
    * (Release build): `cmake ..` 
    * (Debug build): `cmake -DCMAKE_BUILD_TYPE=Debug ..` 
* Run the build command: `cmake --build .`