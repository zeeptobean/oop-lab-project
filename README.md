# OOP lab project: Library manager
**Class: 24C06**
**Group 3 - 24C06**  
**Members**
- Lê Minh Đức - 24127025
- Đoàn Quốc Bảo - 24127326
- Lê Trần Quang Huy - 24127396 
- Tôn Thất Nhật Minh - 24127083

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

### Step 1. Compilation
Simply running Xmake on base directory
```
xmake build release
```

Binaries reside in `bin` directory. Note that it must be in `bin` directory, with `data` directory located outside in order to load application data

The source tree should be like this 

```
<base-directory>/
├── bin/    # binary file
├── data/   # app data
```

## Compiling on Linux

### Step 0. Install Xmake & toolchain as in prerequisites

### Step 1a. Install SDL3 and SDL3_image using prebuilt binaries (on newer distros only):
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

Binaries reside in `bin` directory. Note that it must be in `bin` directory, with `data` directory located outside in order to load application data

The source tree should be like this 

```
<base-directory>/
├── bin/    # binary file
├── data/   # app data
```