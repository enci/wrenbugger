# wrenbugger

🐛 Terminal Debugger for Wren using FTXUI

## About

Wrenbugger is a terminal-based debugger for the Wren programming language, built with FTXUI for a modern, interactive debugging experience.

## Features

- 📄 Source code viewer with line numbers
- 🔍 Variable inspection panel
- 📚 Call stack display
- 🎨 Clean, modern terminal UI
- ⌨️ Keyboard shortcuts for easy navigation

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- C99 compatible compiler
- Git

### Clone with Submodules

```bash
git clone --recursive https://github.com/enci/wrenbugger.git
cd wrenbugger
```

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build . -j$(nproc)
```

The executable will be at `build/code/wrenbugger`.

## Usage

```bash
./build/code/wrenbugger <script.wren>
```

Example:

```bash
./build/code/wrenbugger example.wren
```

### Controls

- **Q** or **ESC** - Quit the debugger
- Arrow keys - Navigate through UI panels

## Project Structure

```
wrenbugger/
├── code/           # Debugger source code
├── external/       # Git submodules
│   ├── wren/      # Wren VM (debugger branch)
│   └── ftxui/     # FTXUI library
├── build/         # Build directory (gitignored)
└── example.wren   # Sample Wren script
```

## Coding Style

This project follows the xxs coding style:
- `lowercase_with_underscores` for identifiers
- `#pragma once` for header guards
- 🎯 Emoji prefixes in commit messages (small caps)

## License

See LICENSE file for details.