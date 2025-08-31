# OOP lab project: Library manager
**Class: 24C06**
**Group 3 - 24C06**  
**Members**
- Lê Minh Đức - 24127025
- Đoàn Quốc Bảo - 24127326
- Lê Trần Quang Huy - 24127396 
- Tôn Thất Nhật Minh - 24127083

## Source tree

```
<base-directory>/
├── bin/                            # binary file (after building)
├── data/                           # app data
├── imgui-sdl3/                     # ImGui library
├── include/                        # include directory
    ├──core/                        # core app header files
    ├──gui/                         # gui header files
    ├──nlohmann/                    # nlohmann/json library
    └──*.hpp                        # other header files
├── SDL3                            # SDL3 library, Windows only
├── SDL3_image                      # SDL3_image library, Windows only
├── src/                            # include directory
    ├──core/                        # core app source files
    ├──gui/                         # gui source files
    ├──event_dispatcher.cpp         # gui notify mechanism
    ├──main.cpp                     # main class
    ├──sha256.cpp                   # SHA256 hash function, self-implemented
    └──texture_cache.cpp            # caching book images when drawing gui 
├── xmake.lua                       # Xmake build script
├── .gitignore
└── README.md
```

## Dependencies
- [nlohmann/json](https://github.com/nlohmann/json) (included)
- [Dear ImGui](https://github.com/ocornut/imgui) (included, using SDL3renderer backend)
- [SDL](https://github.com/libsdl-org/SDL) 3.20 or later (static library included for Windows only)
- [SDL3_image](https://github.com/libsdl-org/SDL_image) (static library included for Windows only)

## Toolchain prerequisites
- C++17-compatible GCC compiler 
- [Xmake](https://github.com/xmake-io/xmake) 3.0 or later 

## Compiling on Windows

### Step 0. Install Xmake & toolchain as in prerequisites

### Step 1a. Compilation on Windows x86-64
Simply running Xmake on base directory
```
xmake build release
```

Binaries reside in `bin` directory. Note that it must be run in `bin` directory, with `data` directory located outside in order to load application data

You will need to install image processing libraries (libpng for PNG, libjpeg-turbo for jpeg, ...). Consult the [SDL3_image CMake tutorial](https://github.com/libsdl-org/SDL_image/blob/main/docs/INTRO-cmake.md) for more detail

Prebuilt binaries is available in [Release](https://github.com/zeeptobean/oop-lab-project/releases), with libpng and libjpeg-turbo included

### Step 1b. Compilation on Windows x86 or arm64

See Compiling on Linux, from step 1b

## Compiling on Linux

### Step 0. Install Xmake & toolchain as in prerequisites

### Step 1a. Install SDL3 and SDL3_image using prebuilt packages (on newer distros only):
#### Ubuntu 25.04/Debian 13 or later
```
sudo apt install libsdl3-dev libsdl3-image-dev
```

#### Arch linux
```
sudo pacman -S sdl3
yay -S sdl3_image
```
SDL3 from Arch do not support static linking

### Step 1b. Build SDL3 and SDL3_image from sources
Consult [SDL3 repository](https://github.com/libsdl-org/SDL) and [SDL3_image repository](https://github.com/libsdl-org/SDL_image) for build instruction

### Step 2. Compilation
Run Xmake on base directory
```
xmake build release
```
By default Linux & MacOS target do not static on release build (due to dependencies problems). This can be modified in `xmake.lua`

Binaries reside in `bin` directory. Note that it must be run in `bin` directory, with `data` directory located outside in order to load application data

### Step 3. Check for image libraries for SDL3_image
SDL3_image in either static or dynamic build will dynamically detect image processing libraries like libpng, libibjpeg-turbo, libwebp, ...

If you built the library by youself, double check if these libraries present 

If you installed prebuilt packages, these dependencies are included

## Disclaimer and Licensing

The binaries and source code are released for educational purposes

The images of book covers used within this application are for demonstration and educational purposes only. These images are intended to provide a realistic visual representation of a library system's interface and are not intended for commercial use or distribution. The copyright for all book covers and related artwork belongs to their respective publishers, authors, and artists. No ownership of these images is claimed or implied by the creators of this project.