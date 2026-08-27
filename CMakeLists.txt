cmake_minimum_required(VERSION 3.25)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# ==============================================================================
# Project Setup
# ==============================================================================
project(Midgard
    VERSION 0.1.0
    DESCRIPTION "Modern C++20 3D Rendering Engine with Vulkan & OpenGL"
    LANGUAGES C CXX
)

# Enforce C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compilation database for LSP / IDE IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable FetchContent module
include(FetchContent)

# ==============================================================================
# System Dependencies
# ==============================================================================

# Vulkan SDK
message(STATUS "[Midgard] Searching for Vulkan SDK...")
find_package(Vulkan REQUIRED)

find_library(SHADERC_LIB NAMES shaderc_shared shaderc_combined HINTS $ENV{VULKAN_SDK}/lib)
if(SHADERC_LIB)
    message(STATUS "[Midgard] Found shaderc library: ${SHADERC_LIB}")
    add_library(shaderc INTERFACE)
    target_link_libraries(shaderc INTERFACE "${SHADERC_LIB}")
    target_include_directories(shaderc INTERFACE "${Vulkan_INCLUDE_DIR}")
endif()

# Optional GLSL shader compiler (glslc)
find_program(GLSLC_EXECUTABLE NAMES glslc HINTS ${Vulkan_GLSLC_EXECUTABLE} $ENV{VULKAN_SDK}/bin)
if(GLSLC_EXECUTABLE)
    message(STATUS "[Midgard] Found Vulkan GLSL compiler (glslc): ${GLSLC_EXECUTABLE}")
endif()

# ==============================================================================
# External Libraries (FetchContent)
# ==============================================================================

# 1. GLFW (Windowing & Input)
message(STATUS "[Midgard] Fetching GLFW 3.4...")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# 2. GLM (OpenGL Mathematics)
message(STATUS "[Midgard] Fetching GLM 1.0.1...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# 3. GLAD (OpenGL Loader Generator)
message(STATUS "[Midgard] Fetching GLAD...")
set(GLAD_PROFILE "core" CACHE STRING "" FORCE)
set(GLAD_ALL_EXTENSIONS ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# 4. stb (Header-only Image Loading)
message(STATUS "[Midgard] Fetching stb...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# 5. Dear ImGui (UI & Debug Overlay)
message(STATUS "[Midgard] Fetching Dear ImGui v1.91.8...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw glad Vulkan::Vulkan)

# 6. spdlog (Fast C++ Logging Library)
message(STATUS "[Midgard] Fetching spdlog v1.14.1...")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# 7. vk-bootstrap
message(STATUS "[Midgard] Fetching vk-bootstrap v1.3.296...")
FetchContent_Declare(
    vk-bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG        v1.3.296
)
FetchContent_MakeAvailable(vk-bootstrap)

# ==============================================================================
# Executable & Build Target
# ==============================================================================

# Collect engine source and header files
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.h")

# Define target executable
add_executable(Midgard
    main.cpp
    ${ENGINE_SOURCES}
)

# Target include directories
target_include_directories(Midgard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Target compiler options
if(MSVC)
    target_compile_options(Midgard PRIVATE /W4 /utf-8)
else()
    target_compile_options(Midgard PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_compile_options(Midgard PRIVATE -DDEBUG)
target_compile_definitions(Midgard PRIVATE 
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLFW_INCLUDE_NONE
)

# Target linked libraries
target_link_libraries(Midgard PRIVATE
    glfw
    glad
    glm::glm
    stb
    imgui
    spdlog::spdlog
    Vulkan::Vulkan
    shaderc
    vk-bootstrap::vk-bootstrap
)

# Platform-specific dependencies
if(WIN32)
    target_link_libraries(Midgard PRIVATE opengl32)
endif()
