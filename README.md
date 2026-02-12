# Particle Simulation

![application](app.png)

This is a particle simulation built in C++ using Raylib for a university assignment. We were required to implement the simulation on the CPU and on the GPU to be able to compare the performance gain.

I structured the data into a SoA layout to reduce cache misses and used instanced rendering to reduce rendering overhead. For the GPU implementation I used compute shaders. Each step runs as a separate pass, this applies for both simulation types. The results are fed into the instance renderer, either straight from the GPU or uploaded from the CPU. 

Additionally, we were assigned another project that required building an MMO server with physics integration. To cut down on the implementation time, I created DLL based API, which I then used in the [MMO Server](https://github.com/Rocco2300/mmo-server)

## Requirements

- CMake 3.20 or higher
- MinGW 15.2.0 or equialent

## Dependencies

- Raylib
- GLM

The dependencies are downloaded by CMake (FetchContent)

## Building

```
git clone github.com/Rocco2300/particle-simulation

cd particle-simulation
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .

```


## Running 

You can now run the project with the following command:
```
./particle-simulation.exe <mode>
```

Where you can replace mode with:
- `--gpu` to simulate the world on the GPU
- `--cpu` to simulate the world on the CPU