# Gamers Launcher

NOTE: this is my hobby project and i dont use it for commercial use. This project is just for satisfying my ego.

This launcher has its own `.minecraft` folder which is in `~/.config/GamersLauncher/minecraft`

We have also our launcher config files which are in `~/.config/GamersLauncher/`

By the way, this launcher supports both Windows and Linux. In Windows, it's using
`C:/Users/USERNAME/AppData/Roaming/GamersLauncher`, in Linux, it's `~/.config/GamersLauncher`

## Project Structure

```
build -> build folder
include -> headers to include
lib -> external libraries (so/dll)
src -> source and headers files that be linked staticly
 mgr/ -> manager singletons
 etc/ -> helpers/utils
 main.cpp -> main entry point
qml -> UI files
 Main.qml -> main window
CMakeLists.txt -> cmake
```

### Libraries

- Qt 6.9.1
- [My Logger](https://github.com/ilpenSE/loggercpp)
