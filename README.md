# OBS Speech Timer Plugin

A plugin for OBS Studio that provides a speech timer functionality.

## Dependencies

This project requires OBS Studio dependencies to build. You can download the required dependencies from:

- [OBS Windows Dependencies (2023-04-12-x64)](https://obsproject.com/downloads/dependencies2019.zip)

Download the file and place it in the `.deps` folder as `.deps/windows-deps-2023-04-12-x64.zip`

## Building

1. Download and extract the dependencies as mentioned above
2. Run `setup_env.bat` to set up the build environment
3. Use CMake to generate the build files:
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```
4. Build the project:
```bash
cmake --build build --config Release
```

## Installation

Copy the built plugin files from the `build/Release` directory to your OBS Studio plugins directory.

## Usage

1. In OBS Studio, go to View > Docks
2. Select "Speech Timer" to show the timer dock
3. Use the controls to start, stop, and reset the timer

## License

This project is licensed under the GPL v2 License - see the LICENSE file for details. 