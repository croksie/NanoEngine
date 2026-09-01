# Midgard Engine

Midgard Engine is a multi-API 3D rendering engine.  
Currently, the engine supports **OpenGL 4.5+** and **Vulkan 1.3+**.

---

## Prerequisites

In order to build the project, you will need:
- **CMake 4.2+**
- **vcpkg** ([Click here to see how to install it](#-how-to-setup-vcpkg))
- **A C++20 compiler**:
  - Windows: **MSVC** (Visual Studio 2026 recommended)
  - Linux: **GCC 13+** (or Clang[^1])
- **Vulkan SDK** 

---

## How to Install & Build

### 1. Clone the repository
```bash
git clone https://github.com/croksie/Midgard-Engine.git
cd Midgard-Engine
```

### 2. Choose your CMake preset
```bash
cmake --list-presets
```

### 3. Configure CMake
*Example using the `windows-msvc-debug` preset:*
```bash
cmake --preset windows-msvc-debug
```

### 4. Compile the engine
```bash
cmake --build --preset windows-msvc-debug
```

Once built, you can run the engine from `bin/Debug/Midgard.exe` (or `bin/Release/Midgard.exe`).

---

## How to Setup vcpkg

Follow the instructions below to install and configure `vcpkg` for the project.

### 1. Install vcpkg
```bash
git clone https://github.com/microsoft/vcpkg.git <VCPKG_DIR>
```
> **Note:** Replace `<VCPKG_DIR>` with the path where you want to install vcpkg (e.g., `C:\Dev\vcpkg`).

### 2. Compile vcpkg
- **Windows (PowerShell / CMD):**
  ```powershell
  cd <VCPKG_DIR>
  .\bootstrap-vcpkg.bat
  ```
- **Linux / macOS:**
  ```bash
  cd <VCPKG_DIR>
  ./bootstrap-vcpkg.sh
  ```

### 3. Set the `VCPKG_ROOT` environment variable

- **Windows (PowerShell - Permanent for current user):**
  ```powershell
  [System.Environment]::SetEnvironmentVariable('VCPKG_ROOT', '<VCPKG_DIR>', 'User')
  ```
  *(Restart your terminal or IDE after running this command).*

- **Windows (PowerShell - Temporary session only):**
  ```powershell
  $env:VCPKG_ROOT = "<VCPKG_DIR>"
  ```

- **Linux / macOS (`~/.bashrc` or `~/.zshrc`):**
  ```bash
  export VCPKG_ROOT=<VCPKG_DIR>
  ```

---

[^1]: I do not have tested clang