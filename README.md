# Building and Running

## Prerequisites

- CMake
- AVR-GCC toolchain
- SimulIDE

## Build the Project

Configure the project:

```bash
cmake \
  -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/avr-gcc.cmake
```

Build the firmware:

```bash
cmake --build build
```

## Run the Simulation

1. Ensure **SimulIDE** is installed.
2. Open the simulation project:

```text
.simulide/main-v1.sim1
```

3. In SimulIDE, load the generated firmware:

```text
build/src/metro-express.hex
```

4. Start the simulation.
