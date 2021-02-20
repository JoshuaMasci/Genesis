# Genesis
Genesis is a work-in-progress game engine/framework project, with a focus on large worlds and space simulations.

* OpenGL rendering (Vulkan rendering to come)
* Imgui based level editor application
* Multithreaded Job system
* Double precision coordinate system
* Platform abstracted Rendering and Input systems
* GLTF scene support

## Requirements
* Windows (Linux plaftform support will come later)
* Cmake 3.16.0 or greater
* SDL2 2.0.9
* GLEW 1.12.0
* [Reactphysics3d 0.7.1](https://github.com/DanielChappuis/reactphysics3d) 
* [spdlog 1.5.0](https://github.com/gabime/spdlog)
* [GLM](https://github.com/g-truc/glm)
* [Imgui Docking branch](https://github.com/ocornut/imgui)
* [moodycamel concurrentqueue](https://github.com/cameron314/concurrentqueue)
* [Parallel Hashmap](https://github.com/greg7mdp/parallel-hashmap)
* [tinygltf](https://github.com/syoyo/tinygltf)
* [easy_profiler](https://github.com/yse/easy_profiler)

## Build
1. Install all libraries into /lib/
2. Run CMake

## License
[MIT](https://choosealicense.com/licenses/mit/)
