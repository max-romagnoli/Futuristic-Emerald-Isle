cmake_minimum_required(VERSION 3.0)
project(futuristic_emerald_isle)

find_package(OpenGL REQUIRED)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glad-opengl-3.3/include/
		external/tinygltf-2.9.3/
	external/
		futuristic_emerald_isle/
)

add_executable(futuristic_emerald_isle
		futuristic_emerald_isle/main.cpp
		futuristic_emerald_isle/render/shader.cpp
		futuristic_emerald_isle/utils/load_textures.cpp
		futuristic_emerald_isle/render/axys_xyz.cpp
		futuristic_emerald_isle/render/axys_xyz.h
		futuristic_emerald_isle/render/building.cpp
		futuristic_emerald_isle/render/building.h
		futuristic_emerald_isle/utils/init_glfw_glad.cpp
		futuristic_emerald_isle/utils/init_glfw_glad.h
		futuristic_emerald_isle/utils/camera.cpp
		futuristic_emerald_isle/utils/camera.h
		futuristic_emerald_isle/scene/scene.cpp
		futuristic_emerald_isle/scene/scene.h
		futuristic_emerald_isle/render/skybox.cpp
		futuristic_emerald_isle/render/skybox.h
		futuristic_emerald_isle/render/terrain.cpp
		futuristic_emerald_isle/render/terrain.h
		futuristic_emerald_isle/misc/loaded_tree.cpp
		futuristic_emerald_isle/misc/loaded_tree.h
		futuristic_emerald_isle/misc/generated_tree.cpp
		futuristic_emerald_isle/misc/generated_tree.h
		futuristic_emerald_isle/render/tree.cpp
		futuristic_emerald_isle/render/tree.h
		futuristic_emerald_isle/render/forest.cpp
		futuristic_emerald_isle/render/forest.h
		futuristic_emerald_isle/utils/light_cube.cpp
		futuristic_emerald_isle/utils/utils.cpp
		futuristic_emerald_isle/utils/utils.h
		futuristic_emerald_isle/render/city.cpp
		futuristic_emerald_isle/render/city.h
		futuristic_emerald_isle/render/cars.cpp
		futuristic_emerald_isle/render/cars.h
		futuristic_emerald_isle/render/car.cpp
		futuristic_emerald_isle/render/car.h
		futuristic_emerald_isle/render/bird.cpp
		futuristic_emerald_isle/render/bird.h
		futuristic_emerald_isle/render/birds.cpp
		futuristic_emerald_isle/render/birds.h
)

target_link_libraries(futuristic_emerald_isle
	${OPENGL_LIBRARY}
	glfw
	glad
)
