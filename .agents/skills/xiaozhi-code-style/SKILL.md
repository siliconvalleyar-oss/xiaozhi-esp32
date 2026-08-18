---
name: xiaozhi-code-style
description: C++ code style guide using clang-format
---

# XiaoZhi Code Style Guide

This skill covers the coding standards for the XiaoZhi ESP32 project.

## Formatting Tool

The project uses `clang-format` based on Google C++ style with project-specific tweaks.

### Install clang-format

- **Windows**: `winget install LLVM` or `choco install llvm`
- **Linux**: `sudo apt install clang-format` (Ubuntu/Debian)
- **macOS**: `brew install clang-format`

### Usage

**Format a single file**:
```bash
clang-format -i path/to/your/file.cpp
```

**Format the entire project**:
```bash
find main -iname '*.h' -o -iname '*.cc' | xargs clang-format -i
```

**Check without modifying**:
```bash
clang-format --dry-run -Werror path/to/your/file.cpp
```

### IDE Integration

**VS Code**:
1. Install C/C++ extension
2. Set `C_Cpp.formatting` to `clangFormat`
3. Enable `editor.formatOnSave`

**CLion**:
1. Open `Editor > Code Style > C/C++`
2. Set Formatter to `clang-format`
3. Choose "use the .clang-format file in the project"

## Main Rules

- **Indent**: 4 spaces
- **Line width**: 100 characters max
- **Braces**: attach style (`{` on same line as control statement)
- **Pointers/references**: bind to type (left alignment)
- **Includes**: sorted automatically
- **Access specifiers**: indented by -4 spaces

## Important Notes

1. Format code before committing
2. Don't fix alignment by hand after running clang-format
3. To exclude a block:
```cpp
// clang-format off
your code
// clang-format on
```

## FAQ

- **Formatting fails**: check clang-format version, ensure UTF-8 encoding
- **Output differs**: verify `.clang-format` in project root is picked up, check no parent `.clang-format` overrides it
