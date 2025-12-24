# DOS2 WASD Movement Revamped

A mod for Divinity: Original Sin 2 that allows direct character movement with hotkeys and improves the camera.

> **🔀 Fork Notice:** This is a fork of [Ch4nKyy/DOS2WASD](https://github.com/Ch4nKyy/DOS2WASD) with improvements and bug fixes.

## ✨ What's New in Revamped

- **🐧 Linux/Proton Compatibility** — Full support for Steam Play (Proton) on Linux
- **🔧 SDL-Based Input** — Replaced Windows-only input hooks with cross-platform SDL event handling
- **🐛 Bug Fixes** — Fixed some issues and other input problems
- **🧹 Code Quality** — Improved memory safety and thread safety

## ⌨️ Default Keybinds

| Action | Key | Notes | 
| :--- | :--- | :--- |
| **Toggle Movement** | `Caps Lock` | Toggles between WASD and Mouse movement. |


## 📦 Installation

### Windows

1. Download `DOS2WASD_Revamped.zip` from [Releases](https://github.com/GosuDRM/DOS2WASD-Revamped/releases)
2. Extract the zip — it contains a `bin/` folder
3. Copy the contents of `bin/` to your game's `DefEd/bin/` folder:
   ```
   C:\Program Files (x86)\Steam\steamapps\common\Divinity Original Sin 2\DefEd\bin\
   ```
4. Launch the game!

### Linux (Proton)

1. Download `DOS2WASD_Revamped.zip` from [Releases](https://github.com/GosuDRM/DOS2WASD-Revamped/releases)
2. Extract the zip — it contains a `bin/` folder
3. Copy the contents of `bin/` to your game's `DefEd/bin/` folder:
   ```
   ~/.steam/steam/steamapps/common/Divinity Original Sin 2/DefEd/bin/
   ```
4. Launch the game with Proton!

## Building

### Requirements

- [CMake](https://cmake.org/)
  - Add this to your `PATH`
- [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
- [Vcpkg](https://github.com/microsoft/vcpkg)
  - Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing
  vcpkg
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
  - Desktop development with C++
- [Divinity: Original Sin 2 Steam Distribution](https://store.steampowered.com/app/435150/Divinity_Original_Sin_2__Definitive_Edition/)
  - Add the environment variable `DOS2PATH` with the value as path to game install folder
- [7zip](https://www.7-zip.org/)
  - Install to default dir
  
### Register Visual Studio as a Generator

- Open `x64 Native Tools Command Prompt`
- Run `cmake`
- Close the cmd window

### Building

```
git clone https://github.com/GosuDRM/DOS2WASD-Revamped.git
cd DOS2WASD-Revamped
git submodule init
git submodule update --remote
.\build-release.ps1
```

### Build SDL

```
cd to extern
create a folder named build
cd to build
open a console
cmake ..\SDL2
cmake --build . --config debug|release
```

### Solution Generation (Optional)
If you want to generate a Visual Studio solution, run the following command:
```
.\generate-sln.ps1
```

> ***Note:*** *This will generate a `DOS2WASD.sln` file in the **build** directory.*

### VSCode Intellisense (Optional)

To fix Intellisense in VSCode, do the following:

- Install the extensions ```ms-vscode.cpptools``` and ```ms-vscode.cmake-tools```.
- Build the solution with the cmake tools extension.
- In your ```c_cpp_properties.json```, use ```"configurationProvider": "ms-vscode.cmake-tools"```.
This is the only needed parameter, apart from "name".
