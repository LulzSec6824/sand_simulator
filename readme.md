# Sandsimu - Sand Simulator

A simple sand physics simulator written in C++ using Raylib.

## Features
- Simulate sand particles falling and piling up
- Place stone particles that don't move
- Erase particles with the eraser tool
- Smooth physics simulation at 60 FPS

## Controls
- **Left Click**: Place selected material
- **1**: Select Sand
- **2**: Select Stone
- **3**: Select Eraser (Air)

## Build Instructions

### Prerequisites
- CMake 3.11 or higher
- C++20 compiler
- Raylib (included as submodule)

### Building
```bash
# Clone the repository (with submodules)
git clone --recursive https://github.com/LulzSec6824/sand_simulator.git
cd sand_simulator

# Build with CMake
cmake -B build
cmake --build build

# Run the executable
./build/sandsimu
```

## Project Structure
```
sandsimu/
├── header/         # Header files
│   ├── engine.hpp  # Sand engine class
│   └── particle.hpp # Particle structure and types
├── src/            # Source files
│   ├── engine.cpp  # Sand engine implementation
│   └── main.cpp    # Main entry point
├── raylib/         # Raylib library (submodule)
└── CMakeLists.txt  # CMake configuration
```

## License
This project uses Raylib under its [license](raylib/LICENSE).
