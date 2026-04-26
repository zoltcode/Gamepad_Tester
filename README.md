# Gamepad Diagnostic Tool

A simple C++ tool utilizing SDL3 for gamepad testing and input visualization.

## Features
* Full SDL3 API support
* Axis, stick, and button testing
* Zero external dependencies (uses CMake FetchContent)

## How to Build
1. Clone the repository:
   ```bash
   git clone https://github.com/zoltcode/Gamepad_Tester.git
   cd gamepad_tester
   ```
2. Build the project:
   ```bash
   cmake -B build-release -DCMAKE_BUILD_TYPE=Release
   cmake --build build-release
   ```

## License
This project is licensed under the **Apache 2.0** License. See the `LICENSE.txt` file for details.
