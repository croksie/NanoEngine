cmake_minimum_required(VERSION 3.25)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Nom du projet et version C++
project(NanoEngine VERSION 0.1.0 LANGUAGES C CXX)



# Exiger la norme C++20 pour tout le projet
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Exportation des commandes de compilation (utile pour le protocole LSP / intellisense dans VS Code)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inclusion du module FetchContent natif de CMake
include(FetchContent)

# ==============================================================================
# 1. GLFW (Gestion de fenêtre et événements)
# ==============================================================================
message(STATUS "Fetching GLFW...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
# Désactivation des options inutiles de GLFW pour accélérer la compilation
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ==============================================================================
# 2. GLM (Mathématiques 3D)
# ==============================================================================
message(STATUS "Fetching GLM...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# ==============================================================================
# 3. glad (OpenGL Loader - GLAD 1 C generator)
# ==============================================================================
message(STATUS "Fetching GLAD...")
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
)
# Configuration de GLAD pour OpenGL 4.5 Core Profile
set(GLAD_PROFILE "core" CACHE STRING "" FORCE)
set(GLAD_ALL_EXTENSIONS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glad)


# ==============================================================================
# 4. stb (Chargement d'images - stb_image)
# ==============================================================================
message(STATUS "Fetching stb...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

# Création d'une cible interface pour stb_image car le dépôt est header-only
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# ==============================================================================
# 5. Dear ImGui (Interface utilisateur / Debug UI)
# ==============================================================================
message(STATUS "Fetching Dear ImGui...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# Configuration de la bibliothèque ImGui avec les backends GLFW + OpenGL3
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
# ImGui a besoin de connaître l'API de fenêtrage et le loader OpenGL
target_link_libraries(imgui PUBLIC glfw glad)

# ==============================================================================
# 6. spdlog (Système de Logs)
# ==============================================================================
message(STATUS "Fetching spdlog...")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# ==============================================================================
# Configuration de l'Exécutable du Moteur 3D
# ==============================================================================

# Fichiers sources de votre projet
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(NanoEngine main.cpp ${SOURCES})

# Association des bibliothèques externes à votre exécutable
target_link_libraries(NanoEngine PRIVATE
    glfw
    glad
    glm::glm
    stb
    imgui
    spdlog::spdlog
)

if(WIN32)
    target_link_libraries(NanoEngine PRIVATE opengl32)
endif()

# Dossiers d'en-tête de votre projet
target_include_directories(NanoEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Définition des options du compilateur (Warning levels)
if(MSVC)
    target_compile_options(NanoEngine PRIVATE /W4 /utf-8)
else()
    target_compile_options(NanoEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()