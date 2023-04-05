cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(ComputerGraphics C CXX)

# Slightly modified versions of the files from the cpp-starter-project by Jason Turner:
# https://github.com/lefticus/cpp_starter_project/
include("cmake/CompilerWarnings.cmake") # Enable almost all compiler warnings and CMake option to enable -Werror.
include("cmake/Sanitizers.cmake") # CMake options to enable address, memory, UB and thread sanitizers.
include("cmake/StaticAnalyzers.cmake") # CMake options to enable clang-tidy or cpp-check.

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework")
	# Create framework library and include CMake scripts (compiler warnings, sanitizers and static analyzers).
	add_subdirectory("framework")
else()
	# During development the framework lives in parent folder.
	add_subdirectory("../../../framework/" "${CMAKE_BINARY_DIR}/framework/")
endif()

add_executable(Group8FinalProject "src/main.cpp")
target_compile_features(Group8FinalProject PRIVATE cxx_std_20)
target_link_libraries(Group8FinalProject PRIVATE CGFramework)
enable_sanitizers(Group8FinalProject)
set_project_warnings(Group8FinalProject)

# Copy all files in the resources folder to the build directory after every successful build.
add_custom_command(TARGET Group8FinalProject POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_CURRENT_LIST_DIR}/resources/" "$<TARGET_FILE_DIR:Group8FinalProject>/resources/")

# We would like to copy the files when they changed. Even if no *.cpp files were modified (and
# thus no build is triggered). We tell CMake that the executable depends on the shader files in
# the build directory. We also tell it how to generate those files (by copying them from the
# shaders folder in this directory). The gather all glsl files in the shaders folder when CMake
# is configured. So if you were to add a shader file then you need to configure CMake again.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

set(shader_copies "")
file(GLOB shader_sources "${CMAKE_CURRENT_LIST_DIR}/shaders/*.glsl")
foreach (shader_file IN LISTS shader_sources)
	get_filename_component(file_name ${shader_file} NAME)
	message("shader_file: ${file_name}")
	add_custom_command(
		OUTPUT "${CMAKE_BINARY_DIR}/shaders/${file_name}"
		COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_CURRENT_LIST_DIR}/shaders/${file_name}"
		"${CMAKE_BINARY_DIR}/shaders/${file_name}"
		DEPENDS "${CMAKE_CURRENT_LIST_DIR}/shaders/${file_name}"
		)
	LIST(APPEND shader_copies "${CMAKE_BINARY_DIR}/shaders/${file_name}")
endforeach()
add_custom_target(copy_shaders DEPENDS ${shader_copies})
add_dependencies(Group8FinalProject copy_shaders)