cmake_minimum_required(VERSION 3.0)
project(lab2)

find_package(OpenGL REQUIRED)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glad-opengl-3.3/include/
	external/
	lab2/
)

add_executable(lab2_building
	lab2/lab2_building.cpp
	lab2/render/shader.cpp
		lab2/utils/load_textures.cpp
		lab2/render/axys_xyz.cpp
		lab2/render/axys_xyz.h
		lab2/render/building.cpp
		lab2/render/building.h
		lab2/utils/init_glfw_glad.cpp
		lab2/utils/init_glfw_glad.h
		lab2/utils/camera.cpp
		lab2/utils/camera.h
		lab2/scene/scene.cpp
		lab2/scene/scene.h
		lab2/render/renderable_object.cpp
		lab2/render/renderable_object.h
		lab2/render/skybox.cpp
		lab2/render/skybox.h
)

add_executable(lab2_skybox
		lab2/lab2_skybox.cpp
	lab2/render/shader.cpp
		lab2/lab2_skybox.cpp
)

target_link_libraries(lab2_building
	${OPENGL_LIBRARY}
	glfw
	glad
)

target_link_libraries(lab2_skybox
		${OPENGL_LIBRARY}
		glfw
		glad
)
