# Wrenbugger

A terminal-based debugger for the Wren programming language.

## Features

- Source code viewing with line numbers
- Breakpoint management (add/remove/toggle)
- Step over/into/out debugging
- Stack trace inspection
- Variable inspection
- 8x8 pixel display for simple graphics
- Output log panel
- Keyboard input API

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

```bash
./executable/wrenbugger path/to/script.wren
```

## Controls

- F5: Continue execution
- F10: Step over
- F11: Step into
- Shift+F11: Step out
- Click on line number to toggle breakpoint

## Wren API

Your Wren scripts can use these IO methods:

- `IO.print(text)` - Print to output log
- `IO.input()` - Get keyboard input
- `IO.setPixel(x, y, on)` - Set pixel on 8x8 display
- `IO.clearDisplay()` - Clear the display

## Example

See `examples/test.wren` for a simple interactive demo.
