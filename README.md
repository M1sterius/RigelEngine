# Rigel Engine
A free and open source 3D game engine written in C++. I'm working on this hobby project in my free
time to, first of all, LEARN as much as possible and to have fun. That means that some implementations may be naive and not fully production-ready, so don't just me too hard on that.

## Implemented and planned features
- ✅ Component-based architecture similar to unity
- ✅ Scene system with lifecycle hooks (`OnLoad`, `OnStart`, etc.)
- ✅ Full scene serialization/deserialization
- ✅ Custom event system with support for multithreaded dispatch and user-defined events
- ✅ Custom handle system that allows memory-safe access to key engine objects (`Component`, `GameObject`, `Scene`, `Asset`)
- ✅ Simple input system with both in-game-loop input checks and input events supported
- ✅ Multithreaded transform update
- 🔧 Graphics API agnostic renderer (the rendering system is designed with support for multiple APIs in mind, however currently only Vulkan renderer is implemented and I don't think I have enough time to work on integrating any other API)
- 🔧 Graphics API agnostic ImGUI integration (UI system is also designed with support for multiple APIs in mind but the situation here is similar to the main renderer).

## Project goals

## Project structure overview
The project is divided into 3 targets, all compiled together via CMakeLists file at the root directory. The targets are:
- Engine. This is the core engine project. It is compiled as a static library. A python script handles building assets bundled with the engine (e.g. Shaders) and grouping them into one folder in the output directory.
- Editor. Currently empty. At the very beginning of the project I envisioned editor to be a standalone application that linked the engine core library, however at this moment I'm contemplating emedding the editor into the core library.
- Sandbox. This is just the test project linking the engine.

## Architecture overview
The core engine functionality is divided into modular subsystems managed by central `Engine` class. Only a single instance of any subsystem can exist at a time and it's strictly responsibility of the `Engine` to start up and shut down the subsystems and manage their lifetime. Each subsystem is solely responsible for handling it's part of engine's feature set. 

Entity-Component System is implemented in a way similar to that of Unity engine. Each `Scene` is composed of multiple `GameObjects` and each `GameObject` includes a set of components. Components hold the actual data and subscribe to engine events to manipulate that data.

The event system is centered around the `EventManager` subsystem. A callback (which can be a free function, an std::function or a class method) can be suscribed to any event type derived from Event base class. Components can subscribe to specific events (e.g., GameUpdateEvent, PhysicsTickEvent) to implement game logic and respond to engine-driven updates. Special lifetime events such as `OnLoad`, `OnStart`, `OnDestroy`, `OnEnable` and `OnDisable` bypass the `EventManager` and get invoked by the `GameObject` owning that component directly.

## Requirements
- CMake 3.28+
- C++20-Compatible compiler (gcc or msvc, clang wasn't tested)
- Vulkan SDK 1.4+
- Python3.

All dependencies are built from source that's located in Dependencies directory. You don't need to download anything other than the tools listed above.

## Build
Use command below to generate CMake build files with Ninja:
```cmake -S . -B CMakeBuild -DCMAKE_BUILD_TYPE=Debug -G "Ninja"```
CMAKE_BUILD_TYPE must be `Debug`, `Test` or `Release`.

Build the project with
```ninja -C CMakeBuild```